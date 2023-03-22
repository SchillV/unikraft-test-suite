#include "incl.h"

siginfo_t *infop;

void run(void)
{
	pid_t pid_group;
	pid_t pid_child;
	pid_child = fork();
	if (!pid_child)
		exit(0);
	pid_group = getpgid(0);
	TST_EXP_FAIL(waitid(P_PGID, pid_group+1, infop, WEXITED), ECHILD);
	memset(infop, 0, sizeof(*infop));
	TST_EXP_PASS(waitid(P_PGID, pid_group, infop, WEXITED));
	TST_EXP_EQ_LI(infop->si_pid, pid_child);
	TST_EXP_EQ_LI(infop->si_status, 0);
	TST_EXP_EQ_LI(infop->si_signo, SIGCHLD);
	TST_EXP_EQ_LI(infop->si_code, CLD_EXITED);
}

void main(){
	setup();
	cleanup();
}
