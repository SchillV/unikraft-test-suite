#include "incl.h"

int fd = -1, pid;

struct flock flocks;

int  verify_fcntl(
{
	flocks.l_type = F_RDLCK;
	TST_EXP_PASS(fcntl(fd, F_GETLK, &flocks), "fcntl(%d, F_GETLK, &flocks)", fd);
	TST_EXP_EQ_LI(flocks.l_type, F_UNLCK);
	TST_EXP_EQ_LI(flocks.l_whence, SEEK_CUR);
	TST_EXP_EQ_LI(flocks.l_start, 0);
	TST_EXP_EQ_LI(flocks.l_len, 0);
	TST_EXP_EQ_LI(flocks.l_pid, pid);
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
	cleanup();
}
