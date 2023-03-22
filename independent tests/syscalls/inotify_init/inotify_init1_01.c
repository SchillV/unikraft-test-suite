#include "incl.h"
#define IN_CLOEXEC O_CLOEXEC

void run(void)
{
	int fd, fd_flags;
	TST_EXP_FD(tst_syscall(__NR_inotify_init1, 0));
	fd = TST_RET;
	fd_flags = fcntl(fd, F_GETFD);
	TST_EXP_EQ_LI(fd_flags & FD_CLOEXEC, 0);
	close(fd);
	TST_EXP_FD(tst_syscall(__NR_inotify_init1, IN_CLOEXEC));
	fd = TST_RET;
	fd_flags = fcntl(fd, F_GETFD);
	TST_EXP_EQ_LI(fd_flags & FD_CLOEXEC, FD_CLOEXEC);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
