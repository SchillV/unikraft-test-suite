#include "incl.h"
#define _GNU_SOURCE

volatile int child_exited;

void *child_stack;

int child_fn(void *unused LTP_ATTRIBUTE_UNUSED)
{
	int i;
	for (i = 0; i < 100; i++) {
		sched_yield();
		usleep(1000);
	}
	child_exited = 1;
	_exit(0);
}

int  verify_clone(
{
	child_exited = 0;
	TST_EXP_PID_SILENT(ltp_clone(CLONE_VM | CLONE_VFORK | SIGCHLD, child_fn, NULL,
					CHILD_STACK_SIZE, child_stack), "clone with vfork");
	if (!TST_PASS)
		return;
	TST_EXP_VAL(child_exited, 1);
}

void main(){
	setup();
	cleanup();
}
