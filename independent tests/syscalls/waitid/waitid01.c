#include "incl.h"

siginfo_t *infop;

void run(void)
{
	pid_t pidchild;
	pidchild = fork();
	if (!pidchild)
		exit(123);
	TST_EXP_PASS(waitid(P_ALL, 0, infop, WEXITED));
	TST_EXP_EQ_LI(infop->si_pid, pidchild);
	TST_EXP_EQ_LI(infop->si_status, 123);
	TST_EXP_EQ_LI(infop->si_signo, SIGCHLD);
	TST_EXP_EQ_LI(infop->si_code, CLD_EXITED);
}

void main(){
	setup();
	cleanup();
}
