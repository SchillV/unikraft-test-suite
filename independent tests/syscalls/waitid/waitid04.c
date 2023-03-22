#include "incl.h"

siginfo_t *infop;

void run(void)
{
	pid_t pid_child;
	pid_child = fork();
	if (!pid_child) {
		TST_CHECKPOINT_WAIT(0);
		return;
	}
	memset(infop, 0, sizeof(*infop));
	TST_EXP_PASS(waitid(P_ALL, pid_child, infop, WNOHANG | WEXITED));
	TST_EXP_EQ_LI(infop->si_pid, 0);
	TST_CHECKPOINT_WAKE(0);
}

void main(){
	setup();
	cleanup();
}
