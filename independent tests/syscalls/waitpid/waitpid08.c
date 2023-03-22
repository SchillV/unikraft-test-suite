#include "incl.h"

void do_child_1(void)
{
	pid_t pid;
	int i;
	int status;
	for (i = 0; i < MAXKIDS; i++) {
		if (i == (MAXKIDS / 2))
			setpgid(0, 0);
		pid = fork();
		if (pid == 0)
			do_exit(1);
		fork_kid_pid[i] = pid;
	}
	 * zero
	 */
	if (TST_TRACE(waitpid_ret_test(-1, &status, WNOHANG | WUNTRACED, 0, 0)))
		return;
	TST_CHECKPOINT_WAKE2(0, MAXKIDS);
	if (TST_TRACE(reap_children(-1, WUNTRACED, fork_kid_pid, MAXKIDS)))
		return;
	tst_res(TPASS, "Test PASSED");
}

void main(){
	setup();
	cleanup();
}
