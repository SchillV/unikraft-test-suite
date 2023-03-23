#include "incl.h"

int fd;

char fname[255];

int  verify_fcntl()
{
	int ret = fcntl(fd, F_GETFL, 0);
	if (ret == -1) {
		printf("fcntl(%s, F_GETFL, 0) failed, error number %d\n", fname, errno);
		return 0;
	}
	if ((ret & O_ACCMODE) != O_RDWR) {
		printf("fcntl(%s, F_GETFL, 0) returned wrong access mode %li, expected %i\n", fname, ret & O_ACCMODE, O_RDWR);
		return 0;
	}
	printf("fcntl(%s, F_GETFL, 0) returned %lx\n", fname, ret);
	return 1;
}

void setup(void)
{
	sprintf(fname, "fcntl04_%d", getpid());
	fd = open(fname, O_RDWR | O_CREAT, 0700);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	if(verify_fcntl())
		printf("test succeeded\n");
	cleanup();
}
