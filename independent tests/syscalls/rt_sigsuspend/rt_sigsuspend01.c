#include "incl.h"

void sig_handler(int sig)
{
	(void) sig;
}

int  verify_rt_sigsuspend(
{
	int i;
	sigset_t set, set1, set2;
	struct sigaction act = {.sa_handler = sig_handler};
	if (sigemptyset(&set) < 0)
		tst_brk(TFAIL | TERRNO, "sigemptyset failed");
	rt_sigaction(SIGALRM, &act, NULL, SIGSETSIZE);
	rt_sigprocmask(0, NULL, &set1, SIGSETSIZE);
	alarm(1);
tst_syscall(__NR_rt_sigsuspend, &set, SIGSETSIZE);
	alarm(0);
	if (TST_RET != -1)
		tst_brk(TFAIL, "rt_sigsuspend returned %ld", TST_RET);
	if (TST_ERR != EINTR)
		tst_brk(TFAIL | TTERRNO, "rt_sigsuspend() failed unexpectedly");
	tst_res(TPASS, "rt_sigsuspend() returned with -1 and EINTR");
	rt_sigprocmask(0, NULL, &set2, SIGSETSIZE);
	for (i = 1; i < SIGRTMAX; i++) {
		if (i >= __SIGRTMIN && i < SIGRTMIN)
			continue;
		if (sigismember(&set1, i) != sigismember(&set2, i))
			tst_brk(TFAIL, "signal mask not preserved");
	}
	tst_res(TPASS, "signal mask preserved");
}

void main(){
	setup();
	cleanup();
}
