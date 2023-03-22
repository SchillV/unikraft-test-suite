#include "incl.h"

void *child_stack;

int do_child(void *arg LTP_ATTRIBUTE_UNUSED)
{
	return 0;
}

int  verify_clone(
{
	int status;
	pid_t pid = 0;
	TST_EXP_PID_SILENT(ltp_clone(SIGCHLD, do_child, NULL, CHILD_STACK_SIZE,
				child_stack));
	if (!TST_PASS)
		return;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		tst_res(TPASS, "Using return in child will not cause abnormal exit");
		return;
	}
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV) {
		tst_res(TFAIL, "Use of return in child caused SIGSEGV");
		return;
	}
	tst_res(TFAIL, "Using return 0 in child the child %s", tst_strstatus(status));
}

void main(){
	setup();
	cleanup();
}
