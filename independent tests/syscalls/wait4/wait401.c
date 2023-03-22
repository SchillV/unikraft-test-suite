#include "incl.h"
#define _USE_BSD

void run(void)
{
	pid_t pid;
	int status = 1;
	struct rusage rusage;
	pid = fork();
	if (!pid) {
		TST_PROCESS_STATE_WAIT(getppid(), 'S', 0);
		exit(0);
	}
	TST_EXP_PID_SILENT(wait4(pid, &status, 0, &rusage), "wait4()");
	if (!TST_PASS)
		return;
	if (TST_RET != pid) {
		tst_res(TFAIL, "wait4() returned wrong pid %li, expected %i",
			TST_RET, pid);
	} else {
		tst_res(TPASS, "wait4() returned correct pid %i", pid);
	}
	if (!WIFEXITED(status)) {
		tst_res(TFAIL, "WIFEXITED() not set in status (%s)",
		        tst_strstatus(status));
		return;
	}
	tst_res(TPASS, "WIFEXITED() is set in status");
	if (WEXITSTATUS(status))
		tst_res(TFAIL, "WEXITSTATUS() != 0 but %i", WEXITSTATUS(status));
	else
		tst_res(TPASS, "WEXITSTATUS() == 0");
}

void main(){
	setup();
	cleanup();
}
