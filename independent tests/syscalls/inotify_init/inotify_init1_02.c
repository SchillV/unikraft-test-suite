#include "incl.h"
#define IN_NONBLOCK O_NONBLOCK

void run(void)
{
	int fd, flags;
	TST_EXP_FD(tst_syscall(__NR_inotify_init1, 0));
	fd = TST_RET;
	flags = fcntl(fd, F_GETFL);
	TST_EXP_EQ_LI(flags & O_NONBLOCK, 0);
	close(fd);
	TST_EXP_FD(tst_syscall(__NR_inotify_init1, IN_NONBLOCK));
	fd = TST_RET;
	flags = fcntl(fd, F_GETFL);
	TST_EXP_EQ_LI(flags & O_NONBLOCK, O_NONBLOCK);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
