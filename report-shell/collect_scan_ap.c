#include <stdio.h>
#include <string.h>

#define LINE_MAX_SIZE	1024

int main(int argc, char **argv) {
	char line[LINE_MAX_SIZE] = {0}, *p, *end;
	int i, ap_num[3] = {0};

	while (fgets(line, LINE_MAX_SIZE, stdin) != NULL) {
		//collect scan aps
		p = strstr(line, "CUR=");
		end = strstr(p, ";");
		*end = '\0';
		i = atoi(p + 4);
		ap_num[i >= 2 ? 1: 0]++;
	}

	fprintf(stderr, "\t%d\t%d\n", ap_num[0], ap_num[1]);

	return 0;
}
