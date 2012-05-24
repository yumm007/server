#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int i;
	unsigned long start_mac;
	unsigned char *m_pac = (void *)&start_mac;
	unsigned int mac_char[10] = {0};
	unsigned int *p = (void *)mac_char;
	char cmd[128], file_name[64];

	if (argc != 3) {
		fprintf(stderr, "USAGE %s 7C-6A-DB-03-01-02 100.\n", argv[0]);
		return -1;
	}

	sscanf(argv[1], "%02X-%02X-%02X-%02X-%02X-%02X", p+5, p+4, p+3, p+2, p+1, p+0);
	
	for (i = 0; i < 6; i++) {
		m_pac[i] = p[i];
	}

	for (i = atoi(argv[2]); i > 0; i--, start_mac++) {
		sprintf(file_name, "%02X-%02X-%02X-%02X-%02X-%02X.ini", \
			m_pac[5], m_pac[4], m_pac[3], m_pac[2], m_pac[1], m_pac[0]);
		sprintf(cmd, "touch %s", file_name);
		system(cmd);
		sprintf(cmd, "./make-conf.sh %s", file_name);
		system(cmd);
	}

	return 0;
}
