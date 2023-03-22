#include "incl.h"

int pidfd = -1;

void run(void)
{
	int flag;
	TST_EXP_FD_SILENT(pidfd_open(getpid(), 0), "pidfd_open(getpid(), 0)");
	pidfd = TST_RET;
	flag = fcntl(pidfd, F_GETFD);
	close(pidfd);
	if (!(flag & FD_CLOEXEC))
		tst_brk(TFAIL, "pidfd_open(getpid(), 0) didn't set close-on-exec flag");
	tst_res(TPASS, "pidfd_open(getpid(), 0) passed");
}

void cleanup(void)
{
	if (pidfd > -1)
		close(pidfd);
}

void main(){
	setup();
	cleanup();
}
