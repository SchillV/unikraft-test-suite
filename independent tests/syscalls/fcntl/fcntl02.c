#include "incl.h"

int fd;

char fname[256];

const int min_fds[] = {0, 1, 2, 3, 10, 100};

int verify_fcntl(unsigned int n)
{
	int min_fd = min_fds[n];
	long int ret = fcntl(fd, F_DUPFD, min_fd);
	if (ret == -1) {
		printf("fcntl(%s, F_DUPFD, %i) failed, error number %d\n",
			fname, min_fd, errno);
		return 0;
	}
	if (ret < min_fd) {
		printf("fcntl(%s, F_DUPFD, %i) returned %ld < %i\n",
			fname, min_fd, ret, min_fd);
		return 0;
	}
	printf("fcntl(%s, F_DUPFD, %i) returned %ld\n",
		fname, min_fd, ret);
	close(ret);
	return 1;
}

void setup(void)
{
	sprintf(fname, "fcntl02_%d", getpid());
	fd = open(fname, O_RDWR | O_CREAT, 0700);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	int ok = 1;
	for(int i=0;i<6;++i)
		ok *= verify_fcntl(i);
	if(ok)
		printf("test succeeded");
	cleanup();
}
