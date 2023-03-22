#include "incl.h"
int main(int argc, char **argv)
{
	int ret;
	int fd;
	if (argc != 2) {
		fprintf(stderr, "Only two arguments: %s <fd>\n", argv[0]);
		exit(1);
	}
	fd = atoi(argv[1]);
	ret = write(fd, argv[1], strlen(argv[1]));
	return ret != -1;
}

