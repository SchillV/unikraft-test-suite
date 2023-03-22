#include "incl.h"

void run(void)
{
	pid_t pid;
	int status;
	pid = fork();
	if (!pid) {
		raise(SIGALRM);
		exit(0);
	}
	TST_EXP_PID_SILENT(waitpid(pid, &status, 0));
	if (!TST_PASS)
		return;
	if (TST_RET != pid) {
		tst_res(TFAIL, "waitpid() returned wrong pid %li, expected %i",
			TST_RET, pid);
	} else {
		tst_res(TPASS, "waitpid() returned correct pid %i", pid);
	}
	if (!WIFSIGNALED(status)) {
		tst_res(TFAIL, "WIFSIGNALED() not set in status (%s)",
		        tst_strstatus(status));
		return;
	}
	tst_res(TPASS, "WIFSIGNALED() set in status");
	if (WTERMSIG(status) != SIGALRM) {
		tst_res(TFAIL, "WTERMSIG() != SIGALRM but %s",
		        tst_strsig(WTERMSIG(status)));
		return;
	}
	tst_res(TPASS, "WTERMSIG() == SIGALRM");
}

void main(){
	setup();
	cleanup();
}
