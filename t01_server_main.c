#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/select.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "t01_server.h"
#include "apply_config.h"
#include "send_recv.h"

#define BUF_SIZE	1024

int sock_d, cmd_d;
struct id_arr_t *id_arr;

//	ret 0: 	select timeout
//	ret 1:	socket fd ready
//	ret 2:	init fd ready
//	ret 3:	cocket and init fd ready

static int wait_for_select(void) {
    struct timeval select_timeout = {3, 0};
    fd_set fdread;
    int ret1 = 0,ret2, max_fd;
    
    FD_ZERO(&fdread);
    FD_SET(cmd_d, &fdread);	//do not swap those 2 line
    FD_SET(sock_d, &fdread);
    select_timeout.tv_sec = 3;
	
	max_fd = sock_d > cmd_d ? sock_d : cmd_d;
	max_fd++;

    if ( select(max_fd, &fdread, NULL, NULL, &select_timeout)  <= 0)
		return 0;
    ret2 = FD_ISSET(sock_d, &fdread) ? 1 : 0;

    return ((ret1 << 1) | ret2);
    
}

static void print_online(void) {
	int on, off, i, max;
	time_t next_time, new_time;
	FILE *fp = LOG_FDS[TOTAL_ONLINE_LOG_FILE_ID];

	fprintf(fp, "\n---------------");print_timestr(fp);fprintf(fp, "TOTAL IDs\t-----------\n");

	max = atoi(CONF_FILTER[TOTAL_CLIENT_LIMIT_ID]);
	time(&new_time);

	for (i = 0, on = 0, off = 0; i < max; i++ ) {
		next_time = id_arr[i].next_wakeup;
		if (next_time != 0 && next_time > new_time -2) {
			on++;
			fprintf(fp, "%d ON;\t", i);
		} else if (next_time != 0) {
			off++;
			fprintf(fp, "%d OFF;\t", i);
		}
	}

	fprintf(fp, "\n--------------TOTAL %d ON %d OFF-------------\n", on, off);

}
void print_timestr(FILE *fp) {
	time_t timep;
	struct tm *p; 

    time(&timep);
    p = localtime(&timep);

    fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d\t",\
		        1900+p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    return;
}


int main(int argc, char *argv[]) {
    int sel_ret;

    apply_config(CONF_FILE);
	if ((id_arr = malloc(sizeof(struct id_arr_t) * atoi(CONF_FILTER[TOTAL_CLIENT_LIMIT_ID]))) == NULL) {
		fprintf(stderr, "malloc failed.\n");
		return -1;
	}
	
    while (1) {

		print_online();

		sel_ret = wait_for_select();
	
		switch (sel_ret) {
	    case 1:
			process_socket(sock_d);
			break;
	    case 2:
			process_socket(cmd_d);
			break;
	    case 3:
			process_socket(sock_d);
			process_socket(cmd_d);
			break;
	    default:
			break;	
		}
		
		fflush(NULL);
    }

    close_config();
	free(id_arr);

    return 0;
}
