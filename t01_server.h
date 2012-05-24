#ifndef __T01_SERVER_H__
#define __T01_SERVER_H__

//update from linux
#define CONF_FILE	"conf.ini"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

struct id_arr_t {
	int                 id_num;
    char                name[32];
    struct sockaddr_in  address;
    time_t              next_wakeup;
	time_t				last_recived;
};


extern struct id_arr_t *id_arr;
extern int sock_d, cmd_d;
void print_timestr(FILE *fp);

#endif

