#include "incl.h"
int main(int argc, char **argv)
{
	int fd;
	if (argc != 2) {
		fprintf(stderr, "Only two arguments: %s <fd>\n", argv[0]);
		exit(1);
	}
	fd = atoi(argv[1]);
	if (fcntl(fd, F_GETFL) < 0 && errno == EBADF)
		return 0;
	return 1;
}

