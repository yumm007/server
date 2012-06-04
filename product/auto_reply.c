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


#define LISTEN_PORT 5081

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

int main(int argc, char *argv[]) {
	int sd;
	char buf[1024];
	struct sockaddr_in ca;
	socklen_t sa_len = sizeof(ca);

	if ((sd = open_socket()) < 0)
		return -1;

	memset(buf, 0, sizeof(buf));
	while (1) {
		if (wait_for_select(sd)) {
			recvfrom(sd, buf, sizeof(buf), 0, (void *)&ca, &sa_len);
			fprintf(stderr, "recv one package:%s.\n", buf);
			sendto(sd, buf, strlen(buf), 0, (void *)&ca, sizeof(ca));
			memset(buf, 0, sizeof(buf));
		}
	}

	close(sd);
	return 0;
}
