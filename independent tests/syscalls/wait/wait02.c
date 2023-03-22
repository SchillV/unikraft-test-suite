#include "incl.h"

int  verify_wait(
{
	int status, exit_child = 1;
	pid_t fpid;
	fpid = fork();
	if (fpid == 0)
		exit(exit_child);
	TST_EXP_PID_SILENT(wait(&status));
	if (!TST_PASS)
		return;
	if (fpid != TST_RET) {
		tst_res(TFAIL, "wait() returned pid %ld, expected %d",
			TST_RET, fpid);
		return;
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == exit_child) {
		tst_res(TPASS, "wait() succeeded");
		return;
	}
	tst_res(TFAIL, "wait() reported child %s", tst_strstatus(status));
}

void main(){
	setup();
	cleanup();
}
