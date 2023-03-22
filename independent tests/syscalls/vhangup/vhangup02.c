#include "incl.h"

void run(void)
{
	pid_t pid, pid1;
	pid = fork();
	if (pid > 0) {
		waitpid(pid, NULL, 0);
	} else {
		pid1 = setsid();
		if (pid1 < 0)
			tst_brk(TBROK | TTERRNO, "setsid failed");
tst_syscall(__NR_vhangup);
		if (TST_RET == -1)
			tst_res(TFAIL | TTERRNO, "vhangup() failed");
		else
			tst_res(TPASS, "vhangup() succeeded");
	}
}

void main(){
	setup();
	cleanup();
}
