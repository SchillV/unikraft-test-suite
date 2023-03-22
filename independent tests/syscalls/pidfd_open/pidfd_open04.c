#include "incl.h"
#ifndef P_PIDFD
#define P_PIDFD  3
#endif

int pidfd = -1;

void run(void)
{
	int flag, pid, ret;
	siginfo_t info;
	pid = fork();
	if (!pid) {
		TST_CHECKPOINT_WAIT(0);
		exit(EXIT_SUCCESS);
	}
	TST_EXP_FD_SILENT(pidfd_open(pid, PIDFD_NONBLOCK),
				"pidfd_open(%d,  PIDFD_NONBLOCK)", pid);
	pidfd = TST_RET;
	flag = fcntl(pidfd, F_GETFL);
	if (!(flag & O_NONBLOCK))
		tst_brk(TFAIL, "pidfd_open(%d, O_NONBLOCK) didn't set O_NONBLOCK flag", pid);
	tst_res(TPASS, "pidfd_open(%d, O_NONBLOCK) sets O_NONBLOCK flag", pid);
	TST_EXP_FAIL(waitid(P_PIDFD, pidfd, &info, WEXITED), EAGAIN,
			"waitid(P_PIDFD,...,WEXITED)");
	TST_CHECKPOINT_WAKE(0);
	ret = TST_RETRY_FUNC(waitid(P_PIDFD, pidfd, &info, WEXITED), TST_RETVAL_EQ0);
	if (ret == 0) {
		tst_res(TPASS, "waitid(P_PIDFD) succeeded after child process terminated");
	} else {
		tst_res(TFAIL, "waitid(P_PIDFD) failed after child process terminated");
		wait(NULL);
	}
	close(pidfd);
}

void setup(void)
{
	pidfd_open_supported();
pidfd_open(getpid(), PIDFD_NONBLOCK);
	if (TST_RET == -1) {
		if (TST_ERR == EINVAL) {
			tst_brk(TCONF, "PIDFD_NONBLOCK was supported since linux 5.10");
			return;
		}
		tst_brk(TFAIL | TTERRNO,
			"pidfd_open(getpid(),PIDFD_NONBLOCK) failed unexpectedly");
	}
	close(TST_RET);
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
