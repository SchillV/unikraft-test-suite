#include "incl.h"
#define STR "abc"
char *TCID = "openat02_child";
int main(int argc, char **argv)
{
	int fd;
	int ret;
	if (argc != 2) {
		fprintf(stderr, "%s <fd>\n", argv[0]);
		exit(1);
	}
	fd = atoi(argv[1]);
	ret = write(fd, STR, sizeof(STR) - 1);
	return ret != -1;
}

