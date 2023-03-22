#include "incl.h"

siginfo_t *infop;

void run(void)
{
	pid_t pidchild;
	pidchild = fork();
	if (!pidchild) {
		pause();
		return;
	}
	kill(pidchild, SIGKILL);
	TST_EXP_PASS(waitid(P_ALL, 0, infop, WEXITED));
	TST_EXP_EQ_LI(infop->si_pid, pidchild);
	TST_EXP_EQ_LI(infop->si_status, SIGKILL);
	TST_EXP_EQ_LI(infop->si_signo, SIGCHLD);
	TST_EXP_EQ_LI(infop->si_code, CLD_KILLED);
}

void main(){
	setup();
	cleanup();
}
