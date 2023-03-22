#include "incl.h"

void catchsig(int sig)
{
	(void)sig;
}

struct tcase {
	void (*sighandler)(int i);
	int kill;
} tcases[] = {
	{SIG_IGN, 0},
	{SIG_DFL, 0},
	{catchsig, 0},
	{SIG_IGN, 1},
	{SIG_DFL, 1},
	{catchsig, 1},
};

void do_test(unsigned int n)
{
	pid_t pid;
	int res;
	struct tcase *tc = &tcases[n];
	pid = fork();
	if (!pid) {
		if (tc->kill) {
			signal(SIGKILL, tc->sighandler);
			pause();
		}
		TST_EXP_FAIL2((long)signal(SIGKILL, tc->sighandler), EINVAL);
		return;
	}
	if (!tc->kill) {
		waitpid(pid, &res, 0);
		return;
	}
	TST_PROCESS_STATE_WAIT(pid, 'S', 0);
	kill(pid, SIGKILL);
	waitpid(pid, &res, 0);
	if (WIFSIGNALED(res))
		TST_EXP_EQ_SSZ(WTERMSIG(res), SIGKILL);
	else
		tst_res(TFAIL, "Child not killed by signal");
}

void main(){
	setup();
	cleanup();
}
