#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <arpa/inet.h>

#include "t01_server.h"
#include "send_recv.h"
#include "apply_config.h"

#define BUF_SIZE	1024

static PKG_MTH_TYPE_T get_mth_type(char *data) {
	if (memcmp(data, PKG_MTH_SND_LCT_S, sizeof(PKG_MTH_SND_LCT_S)) == 0)
		return PKG_MTH_SND_LCT;
   else if (memcmp(data, PKG_MTH_GET_DT_S, sizeof(PKG_MTH_GET_DT_S)) == 0)
		return PKG_MTH_GET_DT;
   else if (memcmp(data, PKG_MTH_GET_LCT_S, sizeof(PKG_MTH_GET_LCT_S)) == 0)
		return PKG_MTH_GET_LCT;
   else if (memcmp(data, PKG_MTH_GET_PSN_S, sizeof(PKG_MTH_GET_PSN_S)) == 0)
		return PKG_MTH_GET_PSN;
   else if (memcmp(data, PKG_MTH_GET_DEV_CONF_S, sizeof(PKG_MTH_GET_DEV_CONF_S)) == 0)
		return PKG_MTH_GET_DEV_CONF;
    
   return PKG_MTH_UNKN;
}

void open_socket(void) {
	struct sockaddr_in sa;

	if ((sock_d = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "create socket error.\n");
		return;
	}

	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(5080);		//should be from configrution file

	if (bind(sock_d, (const struct sockaddr *)&sa, sizeof(sa)) < 0) {
		fprintf(stderr, "bind error.\n");
		close(sock_d);
		sock_d = -1;
	}

	if ((cmd_d = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "create socket error.\n");
		return;
	}

	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(5069);		//should be from configrution file

	if (bind(cmd_d, (const struct sockaddr *)&sa, sizeof(sa)) < 0) {
		fprintf(stderr, "bind error.\n");
		close(cmd_d);
		close(sock_d);
		sock_d = -1;
	}

	return;
}

static void make_datetime(char *buf) {
	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep);

	sprintf(buf, "[DataTime]Year=%02d;Month=%02d;Day=%02d;Hour=%02d;Minute=%02d;Second=%02d;",\
		1900+p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	return;
}

static char *cfg_char[] = { 
    "CFG_VER", "PRD_ID", "MAC_ADDR", "USER_ID", "USER_NAME",
    "TRACKER_SERVER", "TRACKER_PORT", "PROXY_SERVER", "PROXY_PORT",
    "DEF_NW_SSID", "DEF_NW_CH", "STATIC_IP_FLAG",  "DEF_IP_ADDR",
    "DEF_NET_MASK", "DEF_NET_SW", "SENSER_TIMER", "USER_PWD",
    "SCAN_NW_SSID", "SCAN_NW_CH", NULL,
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

#define FILE_PATH_MAX	256
#define CONF_LINE_MAX	256
static void process_one_line(char *line, char *tar, int tar_size) {
    int i;
    size_t len, size;
    char *start;

    if  ((i = check_valid_cfg(line, cfg_char)) == -1)
    	return; 

    for (start = line; *start != '=' ; start++)
    	;
    start++;
    for (; *start == ' ' || *start == '\t'; start++)
    	;
    for (len = 0; !isblank((int)start[len]) && !iscntrl((int)start[len]); len++)
    	;
    if (len >= CONF_LINE_MAX)
    	len = CONF_LINE_MAX;

    size = strlen(tar);
    if (size + len + 1 >= tar_size)
    	return;

    strcat(tar, cfg_char[i]);
    strcat(tar, "=");
    strncat(tar, start, len);
    strcat(tar, ";");

    return;
}

static int get_config(char *id, char *buf, int buf_size) {
	char file_path[FILE_PATH_MAX], line[CONF_LINE_MAX];
	FILE *fp;

	snprintf(file_path, FILE_PATH_MAX -1, "%s/%s.ini",\
			(char *)CONF_FILTER[DEVICE_CONFIG_FILE_DIR_ID], id);
	if ((fp = fopen(file_path, "r")) == NULL) {
		perror(file_path);
		return -1;
	}

	memset(buf, 0, buf_size);
	while (CONF_LINE_MAX && fgets(line, CONF_LINE_MAX, fp) != NULL)
		process_one_line(line, buf, buf_size);

	fprintf(stderr, "send conf file %s for client ", file_path);

	return 0;
}

static void cal_value_from_ap(char *buf) {
	//fprintf(stderr, "%s\n", buf);
}

void process_socket(int sd) {
	char buf[BUF_SIZE];
	struct sockaddr_in ca;
	socklen_t sa_len = sizeof(ca);
	NW_PKG_T *pkg = (void *)buf;

	memset(buf, 0, BUF_SIZE);

	recvfrom(sd, buf, BUF_SIZE, 0, (void *)&ca, &sa_len);
		
	if (memcmp(buf, "mac(wlan0)", 9) == 0) {
		cal_value_from_ap(buf);
		return;
	}

	switch (pkg->header.pkg_type) {
		case LOGIN_REQ:
			break;
		case SND_MSG:
			pkg->header.pkg_type = MSG_RET;
			print_timestr(LOG_FDS[CLIENT_ON_OFF_LOG_FILE_ID]);
			fprintf(LOG_FDS[CLIENT_ON_OFF_LOG_FILE_ID], "%s\n", (char *)pkg->msg_data.msg_val);
			pkg->msg_ret = MSG_DEL_SER;
			sendto(sd, buf, 1024, 0, (void *)&ca, sizeof(ca));

			break;
		case MSG_RET:
			break;
		case SND_MTH:
			switch (get_mth_type((char *)pkg->mth_data.pkg_mth)) {
				case PKG_MTH_SND_LCT:
					print_timestr(LOG_FDS[LOCATION_DATA_LOG_FILE_ID]);
					fprintf(LOG_FDS[LOCATION_DATA_LOG_FILE_ID], "%s;", (char *)pkg->mth_data.mth_val);
					{
						int scan_total, scan_cur;
						sscanf((char *)&pkg->id.pkg_guid[20], "%04d%04d", &scan_total, &scan_cur);
						fprintf(LOG_FDS[LOCATION_DATA_LOG_FILE_ID], "SCAN=%2d;CUR=%2d;", scan_total, scan_cur);
					}
					{
						time_t now_time;
						unsigned long next_time;
						float bat = 4.0;

						time(&now_time);
						
						if (id_arr[atoi((char *)pkg->id.pkg_from)].last_recived != 0)
							fprintf(LOG_FDS[LOCATION_DATA_LOG_FILE_ID], "ELS=%04lld;\n", (long long) now_time - \
									id_arr[atoi((char *)pkg->id.pkg_from)].last_recived );
						else
							fprintf(LOG_FDS[LOCATION_DATA_LOG_FILE_ID], "\n");

						id_arr[atoi((char *)pkg->id.pkg_from)].last_recived = now_time;

						bat = atof(strstr((char *)pkg->mth_data.mth_val, "Battery=") + strlen("Battery="));
						if (bat >= 4.0)
							next_time = 10;
						else if (bat >= 3.8 && bat < 4.0)
							next_time = 20;
						else if (bat >= 3.6 && bat < 3.8)
							next_time = 60;
						else
							next_time = 600;

						next_time *= atoi(CONF_FILTER[CLIENT_OFFLINE_PERIOD_ID]);
							
						id_arr[atoi((char *)pkg->id.pkg_from)].next_wakeup = (unsigned long)now_time + next_time;
					}
					break;
				case PKG_MTH_GET_DT:
					make_datetime((char *)pkg->mth_data.mth_val);
					pkg->header.pkg_type = MTH_RET;
					sendto(sd, buf, 1024, 0, (void *)&ca, sizeof(ca));
					print_timestr(LOG_FDS[CLIENT_ON_OFF_LOG_FILE_ID]);
					fprintf(LOG_FDS[CLIENT_ON_OFF_LOG_FILE_ID], "%s\t Sync Data Time\n", \
						(char *)pkg->id.pkg_from);
					break;
				case PKG_MTH_GET_DEV_CONF:
					pkg->header.pkg_type = MTH_RET;
					print_timestr(stderr);
					//memset((char *)pkg->mth_data.mth_val, 0, sizeof(pkg->mth_data.mth_val));
					//if (get_config(get_mac(inet_ntoa(ca.sin_addr)), (char *)pkg->mth_data.mth_val, 
					if (get_config(get_mac((char *)pkg->mth_data.mth_val), (char *)pkg->mth_data.mth_val, \
						sizeof(pkg->mth_data.mth_val)) == 0) {
						sendto(sd, buf, 1024, 0, (void *)&ca, sizeof(ca));
						fprintf(stderr, "%s from IP %s.\n", (char *)pkg->id.pkg_from, inet_ntoa(ca.sin_addr));
					}
					break;
				default:
					break;
			}
			break;
		case MTH_RET:
			switch (get_mth_type((char *)pkg->mth_data.pkg_mth)) {
				case PKG_MTH_GET_DEV_CONF:
					print_timestr(stderr);
					fprintf(stderr, "send conf file for  %s success.\n", pkg->id.pkg_from);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}
