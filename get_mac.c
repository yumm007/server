#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/if_ether.h>

char * get_mac(const char *ip_addr) {
	int     sockfd;
	unsigned char *ptr;
	struct arpreq arpreq;
	struct sockaddr_in *sin;
	struct sockaddr_storage ss;

	static char mac_buf[20];

	memset(&ss, 0, sizeof(ss));
	memset(&arpreq, 0, sizeof(arpreq));
	memset(mac_buf, 0, sizeof(mac_buf));

	printf("get mac for %s\n", ip_addr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket error");
		return NULL;
	}
	sin = (struct sockaddr_in *) &ss;
	sin->sin_family = AF_INET;
	if (inet_pton(AF_INET, ip_addr, &(sin->sin_addr)) <= 0) {
		perror("inet_pton error");
		close(sockfd);
		return NULL;
	}
	sin = (struct sockaddr_in *) &arpreq.arp_pa;
	memcpy(sin, &ss, sizeof(struct sockaddr_in));
	strcpy(arpreq.arp_dev, "wlan0");
	arpreq.arp_ha.sa_family = AF_UNSPEC;
	if (ioctl(sockfd, SIOCGARP, &arpreq) < 0) {
		perror("ioctl SIOCGARP: ");
		close(sockfd);
		return NULL;
	}
	ptr = (unsigned char *)arpreq.arp_ha.sa_data;
	sprintf(mac_buf, "%02X-%02X-%02X-%02X-%02X-%02X", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));

	close(sockfd);
	return mac_buf;
}

