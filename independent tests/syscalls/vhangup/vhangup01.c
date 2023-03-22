#include "incl.h"

uid_t nobody_uid;

void run(void)
{
	pid_t pid;
	int retval;
	pid = fork();
	if (pid > 0) {
		waitpid(pid, NULL, 0);
	} else {
		retval = setreuid(nobody_uid, nobody_uid);
		if (retval < 0)
			tst_brk(TBROK | TTERRNO, "setreuid failed");
tst_syscall(__NR_vhangup);
		if (TST_RET != -1)
			tst_brk(TFAIL, "vhangup() failed to fail");
		else if (TST_ERR == EPERM)
			tst_res(TPASS, "Got EPERM as expected.");
		else
			tst_res(TFAIL, "expected EPERM got %d", TST_ERR);
	}
}

void setup(void)
{
	struct passwd *pw;
	pw = getpwnam("nobody");
	nobody_uid = pw->pw_uid;
}

void main(){
	setup();
	cleanup();
}
