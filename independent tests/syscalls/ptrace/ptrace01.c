#include "incl.h"

struct tcase {
	int handler;
	int request;
	int exp_wifexited;
	int exp_wtermsig;
	char *message;
} tcases[] = {
	{0, PTRACE_KILL, 0, 9, "Testing PTRACE_KILL without child handler"},
	{1, PTRACE_KILL, 0, 9, "Testing PTRACE_KILL with child handler"},
	{0, PTRACE_CONT, 1, 0, "Testing PTRACE_CONT without child handler"},
	{1, PTRACE_CONT, 1, 0, "Testing PTRACE_CONT with child handler"},
};

volatile int got_signal;

void child_handler(int sig LTP_ATTRIBUTE_UNUSED)
{
	kill(getppidgetppid), SIGUSR2);
}

void parent_handler(int sig LTP_ATTRIBUTE_UNUSED)
{
	got_signal = 1;
}

void do_child(unsigned int i)
{
	struct sigaction child_act;
	if (i == 0)
		child_act.sa_handler = SIG_IGN;
	else
		child_act.sa_handler = child_handler;
	child_act.sa_flags = SA_RESTART;
	sigemptyset(&child_act.sa_mask);
	sigaction(SIGUSR2, &child_act, NULL);
	if ((ptrace(PTRACE_TRACEME, 0, 0, 0)) == -1) {
		tst_res(TWARN, "ptrace() failed in child");
		exit(1);
	}
	kill(getpidgetpid), SIGUSR2);
	exit(1);
}

void run(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	pid_t child_pid;
	int status;
	struct sigaction parent_act;
	got_signal = 0;
	tst_res(TINFO, "%s", tc->message);
	if (tc->handler == 1) {
		parent_act.sa_handler = parent_handler;
		parent_act.sa_flags = SA_RESTART;
		sigemptyset(&parent_act.sa_mask);
		sigaction(SIGUSR2, &parent_act, NULL);
	}
	child_pid = fork();
	if (!child_pid)
		do_child(tc->handler);
	waitpid(child_pid, &status, 0);
	if (((WIFEXITED(status)) && (WEXITSTATUS(status)))
		 || (got_signal == 1))
		tst_res(TFAIL, "Test Failed");
	else if ((ptrace(tc->request, child_pid, 0, 0)) == -1)
		tst_res(TFAIL | TERRNO, "ptrace(%i, %i, 0, 0) failed",
			tc->request, child_pid);
	waitpid(child_pid, &status, 0);
	if ((tc->request == PTRACE_CONT &&
	     WIFEXITED(status) && WEXITSTATUS(status) == tc->exp_wifexited) ||
	    (tc->request == PTRACE_KILL &&
	     WIFSIGNALED(status) && WTERMSIG(status) == tc->exp_wtermsig)) {
		tst_res(TPASS, "Child %s as expected", tst_strstatus(status));
	} else {
		tst_res(TFAIL, "Child %s unexpectedly", tst_strstatus(status));
	}
}

void main(){
	setup();
	cleanup();
}
