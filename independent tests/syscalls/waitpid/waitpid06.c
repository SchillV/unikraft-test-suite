#include "incl.h"

void do_child_1(void)
{
	pid_t pid;
	int i;
	for (i = 0; i < MAXKIDS; i++) {
		if (i == (MAXKIDS / 2))
			setpgid(0, 0);
		pid = fork();
		if (pid == 0)
			do_exit(0);
		fork_kid_pid[i] = pid;
	}
	TST_CHECKPOINT_WAKE2(0, MAXKIDS);
	if (TST_TRACE(reap_children(-1, 0, fork_kid_pid, MAXKIDS)))
		return;
	tst_res(TPASS, "Test PASSED");
}

void main(){
	setup();
	cleanup();
}
