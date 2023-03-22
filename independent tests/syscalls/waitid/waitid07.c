#include "incl.h"

siginfo_t *infop;

void run(void)
{
	pid_t pid_child;
	pid_child = fork();
	if (!pid_child) {
		kill(getpidgetpid), SIGSTOP);
		TST_CHECKPOINT_WAIT(0);
		return;
	}
	memset(infop, 0, sizeof(*infop));
	TST_EXP_PASS(waitid(P_PID, pid_child, infop, WSTOPPED | WNOWAIT));
	TST_EXP_EQ_LI(infop->si_pid, pid_child);
	TST_EXP_EQ_LI(infop->si_status, SIGSTOP);
	TST_EXP_EQ_LI(infop->si_signo, SIGCHLD);
	TST_EXP_EQ_LI(infop->si_code, CLD_STOPPED);
	kill(pid_child, SIGCONT);
	TST_CHECKPOINT_WAKE(0);
}

void main(){
	setup();
	cleanup();
}
