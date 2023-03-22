#include "incl.h"
#define TEST_APP "setpgid03_child"

void do_child(void)
{
	setsid();
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
}

void run(void)
{
	pid_t child_pid;
	child_pid = fork();
	if (!child_pid) {
		do_child();
		return;
	}
	TST_CHECKPOINT_WAIT(0);
	TST_EXP_FAIL(setpgid(child_pid, getppid()), EPERM);
	TST_CHECKPOINT_WAKE(0);
	child_pid = fork();
	if (!child_pid)
		execlp(TEST_APP, TEST_APP, NULL);
	TST_CHECKPOINT_WAIT(0);
	TST_EXP_FAIL(setpgid(child_pid, getppid()), EACCES);
	TST_CHECKPOINT_WAKE(0);
}

void main(){
	setup();
	cleanup();
}
