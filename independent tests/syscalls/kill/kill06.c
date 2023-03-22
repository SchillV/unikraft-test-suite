#include "incl.h"

int  verify_kill(
{
	int nsig, i, status;
	if (!fork()) {
		setpgrp();
		for (i = 0; i < 5; i++) {
			if (!fork())
				pause();
		}
kill(-getpgrp(), SIGKILL);
		if (TST_RET != 0)
			tst_res(TFAIL | TTERRNO, "kill failed");
		exit(0);
	}
	waitpid(-1, &status, 0);
	nsig = WTERMSIG(status);
	if (nsig != SIGKILL) {
		tst_brk(TFAIL, "wait: unexpected signal %d returned, "
			"expected SIGKILL(9)", nsig);
	}
	tst_res(TPASS, "receive expected signal SIGKILL(9)");
}

void main(){
	setup();
	cleanup();
}
