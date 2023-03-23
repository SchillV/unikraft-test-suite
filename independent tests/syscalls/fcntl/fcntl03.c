#include "incl.h"

char fname[255];

int fd;

int  verify_fcntl()
{
	int ret = fcntl(fd, F_GETFD, 0);
	if (ret == -1) {
		printf("fcntl(%s, F_GETFD, 0) failed, error number %d\n", fname, errno);
		return 0;
	}
	printf("fcntl(%s, F_GETFD, 0) returned %d\n", fname, ret);
	return 1;
}

void setup(void)
{
	sprintf(fname, "fcntl03_%d", getpid());
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
