#include "incl.h"

int  verify_getppid(
{
	pid_t proc_id;
	pid_t pid;
	pid_t pproc_id;
	proc_id = getpid();
	pid = fork();
	if (pid == 0) {
		pproc_id = getppid();
		if (pproc_id != proc_id)
			tst_res(TFAIL, "child's ppid(%d) not equal to parent's pid(%d)",
				pproc_id, proc_id);
		else
			tst_res(TPASS, "getppid() returned parent pid (%d)", proc_id);
	}
}

void main(){
	setup();
	cleanup();
}
