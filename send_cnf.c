#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LINE	256
#define MAX_CONF	400
#define PKG_SIZE	556

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

static void process_one_line(char *line, char *tar) {
    int i;
    size_t len, size;
    char *start;

    if  ((i = check_valid_cfg(line, cfg_char)) == -1)
	return;	//这一行没有参数, 否则返回index值

    //从左边的等号开始找起，返回第一个非空格非tab的字符
    for (start = line; *start != '=' ; start++)
	;
    start++;
    for (; *start == ' ' || *start == '\t'; start++)
	;
    //copy截止到后面的第一个非字符
    for (len = 0; !isblank((int)start[len]) && !iscntrl((int)start[len]); len++)
	;
    if (len >= MAX_LINE)
	len = MAX_LINE;

    size = strlen(tar);
    if (size + len + 1 >= MAX_CONF)
	return;

    strcat(tar, cfg_char[i]);
    strcat(tar, "=");
    strncat(tar, start, len);
    strcat(tar, ";");

    return;
}

enum {
    SEND_MSG,
    SEND_CNF,
};

static const char *CMD_STR[] = {
	"msg",
	"conf",
	NULL,
};

enum {
    PKG_TYPE_OFFSET = 8,

    PKG_GUID_OFFSET = 16,
    PKG_FROM_OFFSET = 16+32,
    PKG_TO_OFFSET   = 16+32+32,
    PKG_MTH_OFFSET  = 16+32+32+32,
    PKG_MTHV_OFFSET = 16+32+32+32+32,

    PKG_MSGV_OFFSET = 16+32+32+32,
    PKG_MSGT_OFFSET = 16+32+32+32+400,
};


static int get_cmd_type(const char *cmd) {
	int i;
	for (i = 0; CMD_STR[i] != NULL; i++)
		if (strcmp(cmd, CMD_STR[i]) == 0)
			return i;
	return -1;
}

int main(int argc, char **argv) {
    char line[1024] = {0};
    char data[PKG_SIZE] = {0};
    int sd;
    struct sockaddr_in sa;
    char *p;
    time_t times;
	 pid_t pid;

    if (argc != 4) {
	printf("usage: %s cmd 192.168.0.4 31013\n", argv[0]);
	printf("cmd: conf msg");
	return -2;
    }

    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	perror("socket:");
	return -1;
    }

    bzero(&sa, sizeof(sa));
    sa.sin_family	    = AF_INET;
    sa.sin_addr.s_addr  = inet_addr(argv[2]);
    sa.sin_port	    = htons(atoi(argv[3]));

    times = time(NULL);
	 pid = getpid();
    memcpy(data+PKG_GUID_OFFSET,&times, sizeof(times) );
    memcpy(data+PKG_GUID_OFFSET + 10, &pid, sizeof(pid) );
    strcpy(data+PKG_FROM_OFFSET, "1001=Server");
    strcpy(data+PKG_TO_OFFSET, "nobody");

    switch(get_cmd_type(argv[1])) {
	case SEND_CNF:
	    data[PKG_TYPE_OFFSET ] = 5;	//�����ظ�
	    strcpy(&data[PKG_MTH_OFFSET], "GetDevConfig");
	    while ( fgets(line, 1024, stdin) != NULL)
		process_one_line(line, &data[PKG_MTHV_OFFSET]);
	    break;
	case SEND_MSG:
	    data[PKG_TYPE_OFFSET ] = 2;	//������Ϣ
	    p = &data[PKG_MSGV_OFFSET];
	    fgets(p, 400, stdin);
	    p[strlen(p)-1] = '\0';		//ȥ���س���
	    data[PKG_MSGT_OFFSET] = 0;	//��ͨ����
	    break;
	default:
	    goto ret;
	    break;
    }

    sendto(sd, data, sizeof(data), 0, (void *)&sa, sizeof(sa));

ret:
    close(sd);
    return 0;
}
