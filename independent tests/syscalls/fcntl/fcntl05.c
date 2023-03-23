#include "incl.h"

int fd = -1, pid;

struct flock flocks;

int  verify_fcntl()
{
	int ok = 1;
	flocks.l_type = F_RDLCK;
	int ret = fcntl(fd, F_GETLK, &flocks);
	ok *= (ret != -1);
	if(ok)
		printf("fcntl(%d, F_GETLK, &flocks)\n", fd);
	return ok;
}

void setup(void)
{
	pid = getpid();
	fd = open("filename", O_RDWR | O_CREAT, 0700);
	flocks.l_whence = SEEK_CUR;
	flocks.l_start = 0;
	flocks.l_len = 0;
	flocks.l_pid = pid;
}

void cleanup(void)
{
	if (fd > -1)
		close(fd);
}

void main(){
	setup();
	if(verify_fcntl())
		printf("test succeeded\n");
	cleanup();
}
