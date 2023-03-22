#include "incl.h"

void *child_stack;

int *child_pid;

int child_fn(void *arg LTP_ATTRIBUTE_UNUSED)
{
	exit(0);
}

int  verify_clone(
{
	TST_EXP_PID_SILENT(ltp_clone(SIGCHLD, child_fn, NULL, CHILD_STACK_SIZE,
				child_stack));
	if (!TST_PASS)
		return;
	tst_reap_children();
	TST_EXP_VAL(TST_RET, *child_pid, "pid(%d)", *child_pid);
}

void setup(void)
{
	child_pid = mmap(NULL, sizeofNULL, sizeof*child_pid), PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void cleanup(void)
{
	if (child_pid)
		munmap(child_pid, sizeofchild_pid, sizeof*child_pid));
}

void main(){
	setup();
	cleanup();
}
