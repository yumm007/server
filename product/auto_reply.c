#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <time.h>


#define LISTEN_PORT 5081

static int total_send, max_miss, max_rssi;

static int open_socket(void) {
	struct sockaddr_in sa;
	int sd = -1;

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket:");
		return sd;
	}

	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(LISTEN_PORT);

	if (bind(sd, (const struct sockaddr *)&sa, sizeof(sa)) <0) {
		perror("bind:");
		close(sd);
		return -1;
	}

	return sd;
}

static int wait_for_select(int sd) {
    struct timeval select_timeout = {3, 0};
    fd_set fdread;
    int ret;
    
    FD_ZERO(&fdread);
    FD_SET(sd, &fdread);
    select_timeout.tv_sec = 3;
	
    if ( select(sd+1, &fdread, NULL, NULL, &select_timeout)  <= 0)
		return 0;
    ret = FD_ISSET(sd, &fdread) ? 1 : 0;

    return ret;
}

static void save_log(char *log) {
	char file_name[64] = {0};
	FILE *fp = NULL;
	time_t timep;
	
	memcpy(file_name, log+4, 17);
	strcat(file_name, "-test.log");
	
	fp = fopen(file_name, "a");
	timep = time(NULL);
	fprintf(fp, "\n#Test Environment\n");
	fprintf(fp, "Test_Time=%sSend_Limit=%d\nLoss_Limit=%d\nRSSI_Limit=%ddBm\n", ctime(&timep), total_send, max_miss, max_rssi);
	fprintf(fp, "\n#Test Result\n");
	fprintf(fp, "%s\n", log);

	fclose(fp);
}


/*!
 *\brief auto reply configrution to client when recv any package
 *\param argv[1] total send package of client
 *\param argv[2] max missing package
 *\param argv[3] max rssi value
 *\test ./a.out 1000 2 -56
 */
int main(int argc, char *argv[]) {
	char buf[1024];
	struct sockaddr_in ca;
	socklen_t sa_len = sizeof(ca);
	size_t n;
	int sd;

	if ((sd = open_socket()) < 0)
		return -1;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s 1000 2 -56\n", argv[0]);
		return -1;
	} 

	if ((total_send = atoi(argv[1])) <= 0)
		total_send = 100;
	if ( (max_miss = atoi(argv[2])) > total_send)
		max_miss = total_send;
	if ((max_rssi = atoi(argv[3])) >= 0)
		max_rssi = -65;

	memset(buf, 0, sizeof(buf));
	while (1) {
		if (wait_for_select(sd)) {
			n = recvfrom(sd, buf, sizeof(buf), 0, (void *)&ca, &sa_len);
			if (n > 5) {
				fprintf(stdout, "%s\n", buf);
				save_log(buf);
			} else {
				sprintf(buf, "%04d;%04d;%04d;", total_send, max_miss, max_rssi);
				sendto(sd, buf, strlen(buf), 0, (void *)&ca, sizeof(ca));
			}
			memset(buf, 0, sizeof(buf));
		}
	}

	close(sd);
	return 0;
}
