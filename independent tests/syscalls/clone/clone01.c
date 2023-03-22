#include "incl.h"

void *child_stack;

int do_child(void *arg LTP_ATTRIBUTE_UNUSED)
{
	exit(0);
}

int  verify_clone(
{
	int status, child_pid;
	TST_EXP_PID_SILENT(ltp_clone(SIGCHLD, do_child, NULL,
		CHILD_STACK_SIZE, child_stack), "clone()");
	child_pid = wait(&status);
	if (child_pid == TST_RET)
		tst_res(TPASS, "clone returned %ld", TST_RET);
	else
		tst_res(TFAIL, "clone returned %ld, wait returned %d",
			 TST_RET, child_pid);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		tst_res(TPASS, "Child exited with 0");
	else
		tst_res(TFAIL, "Child %s", tst_strstatus(status));
}

void main(){
	setup();
	cleanup();
}
