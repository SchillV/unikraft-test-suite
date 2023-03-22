#include "incl.h"

void do_compute(void);

void do_fork(void);

void do_sleep(void);

void do_child_1(void)
{
	pid_t pid;
	int i;
	for (i = 0; i < MAXKIDS; i++) {
		pid = fork();
		if (pid == 0) {
			if (i == 0 || i == 1)
				do_exit(0);
			if (i == 2 || i == 3)
				do_compute();
			if (i == 4 || i == 5)
				do_fork();
			if (i == 6 || i == 7)
				do_sleep();
		}
		fork_kid_pid[i] = pid;
	}
	TST_CHECKPOINT_WAKE2(0, MAXKIDS);
	if (TST_TRACE(reap_children(0, 0, fork_kid_pid, MAXKIDS)))
		return;
	tst_res(TPASS, "Test PASSED");
}

void do_compute(void)
{
	int i;
	TST_CHECKPOINT_WAIT(0);
	for (i = 0; i < 100000; i++) ;
	for (i = 0; i < 100000; i++) ;
	for (i = 0; i < 100000; i++) ;
	for (i = 0; i < 100000; i++) ;
	for (i = 0; i < 100000; i++) ;
	for (i = 0; i < 100000; i++) ;
	for (i = 0; i < 100000; i++) ;
	for (i = 0; i < 100000; i++) ;
	for (i = 0; i < 100000; i++) ;
	for (i = 0; i < 100000; i++) ;
	exit(3);
}

void do_fork(void)
{
	pid_t fork_pid;
	int i;
	TST_CHECKPOINT_WAIT(0);
	for (i = 0; i < 50; i++) {
		fork_pid = fork();
		if (fork_pid == 0)
			exit(3);
		if (TST_TRACE(reap_children(fork_pid, 0, &fork_pid, 1)))
			break;
	}
	exit(3);
}

void do_sleep(void)
{
	TST_CHECKPOINT_WAIT(0);
	sleep(1);
	sleep(1);
	exit(3);
}

void main(){
	setup();
	cleanup();
}
