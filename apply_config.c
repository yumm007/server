#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "send_recv.h"
#include "apply_config.h"
#include "t01_server.h"

FILE  *LOG_FDS[LOG_FDS_MAX];

//default value
char CONF_FILTER[MAX_CONF_ID][CONFIG_VAL_MAX_LEN] = {
	{"Linux"},
	{"./"},
	{"/dev/null"},
	{"/dev/null"},
	{"/dev/null"},
	{"/dev/null"},
	{"2000"},
	{"3"},
	{"conf"},
};

static char *CONF_CHAR[] = {
	"OS",\
	"LOG_FILE_DIRECTORY",\
    "TOTAL_ONLINE_LOG_FILE", \
    "LOCATION_DATA_LOG_FILE", \
    "CLIENT_ON_OFF_LOG_FILE",\
    "SEND_MSG_FROM_FILE", \
    "TOTAL_CLIENT_LIMIT",\
    "CLIENT_OFFLINE_PERIOD", \
	"DEVICE_CONFIG_FILE_DIR",\
    NULL,
};

static int check_valid_cfg(char *line, char **cfg_arr) {
    int i;

    for (; *line == ' ' || *line == '\t'; line++)
	;

    for (i = 0; cfg_arr[i] != NULL; i++) {
	if (memcmp(line, cfg_arr[i], strlen(cfg_arr[i])) == 0) 
	    return i;
    }
    return -1;
}


static void process_one_line(char *line) {
    int i, len;
    char *start;

    if  ((i = check_valid_cfg(line, CONF_CHAR)) == -1)
		return;

    //
    for (start = line; *start != '=' ; start++)
		;
    start++;
    for (; *start == ' ' || *start == '\t'; start++)
		;
    
    //
    for (len = 0; !isblank(start[len]) && !iscntrl(start[len]); len++)
		;
    if (len >= CONFIG_VAL_MAX_LEN-4)
		len = CONFIG_VAL_MAX_LEN-4;

    if (len > 0) {
		memset(&CONF_FILTER[i][0], 0, CONFIG_VAL_MAX_LEN);
		strncpy(&CONF_FILTER[i][0], start, len);
    }

    return;
}

static void apply_config_debug(void) {
    int i;
    for (i = 0; i < MAX_CONF_ID; i++)
		fprintf(stderr, "%s = %s \n", CONF_CHAR[i],&CONF_FILTER[i][0]);
    return;
}

static char *append_file_name(char *filename) {
	static char fn[CONFIG_VAL_MAX_LEN] = {0};
	int n;
	time_t t;
	struct tm *tmp;

	memset(fn, 0, sizeof(fn));
	n = sprintf(fn, "%s", CONF_FILTER[LOG_FILE_DIRECTORY_ID]);
	if (fn[n-1] != '/') {
		fn[n] = '/';
		fn[n+1] = '\0';
		n++;
	}

	if (access(fn, F_OK) != 0)
		mkdir(fn, 0777);

	time(&t);
	tmp = localtime(&t);
	
	snprintf(fn + n, CONFIG_VAL_MAX_LEN - n , "%04d-%02d-%02d-%s", tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday, filename);
	return fn;
}

static void open_fds(void) {
	int i;

	for (i = strlen(CONF_FILTER[OS_ID])-1; i >= 0; i--)
		CONF_FILTER[OS_ID][i] = tolower(CONF_FILTER[OS_ID][i]);

	if (strcmp(CONF_FILTER[OS_ID], "windows") == 0 || strcmp(CONF_FILTER[OS_ID], "win") == 0) {
		for (i = LOG_FDS_MIN; i < LOG_FDS_MAX; i++)
			if (CONF_FILTER[i][0] == '\0')
				memcpy(&CONF_FILTER[i][0], "nul", 4);
	}

	LOG_FDS[TOTAL_ONLINE_LOG_FILE_ID] = fopen(append_file_name(&CONF_FILTER[TOTAL_ONLINE_LOG_FILE_ID][0]), "a+");
	LOG_FDS[LOCATION_DATA_LOG_FILE_ID] = fopen(append_file_name(&CONF_FILTER[LOCATION_DATA_LOG_FILE_ID][0]), "a+");
	LOG_FDS[CLIENT_ON_OFF_LOG_FILE_ID] = fopen(append_file_name(&CONF_FILTER[CLIENT_ON_OFF_LOG_FILE_ID][0]), "a+");
	LOG_FDS[SEND_MSG_FROM_FILE_ID] = fopen(&CONF_FILTER[SEND_MSG_FROM_FILE_ID][0], "r");

	cmd_d = fileno(LOG_FDS[SEND_MSG_FROM_FILE_ID]);
	open_socket();
}

void apply_config(const char *conf_file_path){
    FILE *fp;
    char line[1024];

    if ((fp = fopen(conf_file_path, "r")) == NULL) {
		fprintf(stderr, "load config file failed.\n");	
    } else {
		while (fgets(line, 1024, fp) != NULL) {
	    process_one_line(line);
	    memset(line, 0, sizeof(line));
		}
    }

    open_fds();

    apply_config_debug();
    
    return;
}

void close_config(void) {
    int i;

    for (i = LOG_FDS_MIN; i < LOG_FDS_MAX; i++)
	if (LOG_FDS[i] != NULL)
	    fclose(LOG_FDS[i]);

	close(sock_d);

    return;
}

