#include <stdio.h>
#include <string.h>

#define LINE_MAX_SIZE	1024
#define MAX_AP				100

char AP_LIST[MAX_AP][255] = {
	"00:26:48:00:86:02",
	"00:80:48:69:b7:35",
};

static int get_ap_list(char *filename) {
	FILE *fp;
	int i = 0;
	char line[1024];

	if ((fp = fopen(filename, "r")) == NULL) {
		perror("fopen");
		return 0;
	}
	
	while (fgets(line, 1024, fp) != NULL && i < MAX_AP) {
		memcpy(&AP_LIST[i][0], line, 17);
		i++;
	}

	fclose(fp);
	return i;
}

int main(int argc, char **argv) {
	char line[LINE_MAX_SIZE] = {0}, *p;
	int i, n;

	n = get_ap_list("ap.list");

	while (fgets(line, LINE_MAX_SIZE, stdin) != NULL) {
		fprintf(stdout, "%c%c%c%c%c%c%c%c\t", \
			line[11],line[12],line[13],line[14],line[15],line[16],line[17],line[18]);
		for (i = 0; i < n; i++) {
			if ((p = strstr(line, AP_LIST[i])) != NULL) {
				p += strlen(AP_LIST[i]) + 9;
				fprintf(stdout, "%c%c\t", *p, *(p+1));
			} else
				fprintf(stdout, "0\t");
		}
		fprintf(stdout, "\n");
	}

	return 0;
}
