#include "incl.h"
uid_t uid;

int  verify_ptrace(
{
	int child_pid[2];
	child_pid[0] = fork();
	if (!child_pid[0])
		pause();
	child_pid[1] = fork();
	if (!child_pid[1]) {
		setuid(uid);
ptrace(PTRACE_ATTACH, child_pid[0], NULL, NULL);
		if (TST_RET == 0) {
			tst_res(TFAIL, "ptrace() succeeded unexpectedly");
			exit(0);
		}
		if (TST_RET != -1) {
			tst_res(TFAIL,
				"Invalid ptrace() return value %ld", TST_RET);
			exit(0);
		}
		if (TST_ERR == EPERM)
			tst_res(TPASS | TTERRNO, "ptrace() failed as expected");
		else
			tst_res(TFAIL | TTERRNO, "ptrace() expected EPERM, but got");
		exit(0);
	}
	waitpid(child_pid[1], NULL, 0);
	kill(child_pid[0], SIGKILL);
	waitpid(child_pid[0], NULL, 0);
}

void setup(void)
{
	struct passwd *pw;
	pw = getpwnam("nobody");
	uid = pw->pw_uid;
}

void main(){
	setup();
	cleanup();
}
