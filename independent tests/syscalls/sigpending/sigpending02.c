#include "incl.h"

void sigpending_info(void)
{
	switch (tst_variant) {
	case 0:
		tst_res(TINFO, "Testing libc sigpending()");
	break;
	case 1:
		tst_res(TINFO, "Testing __NR_sigpending syscall");
	break;
	case 2:
		tst_res(TINFO, "Testing __NR_rt_sigpending syscall");
	break;
	}
}

int tested_sigpending(sigset_t *sigset)
{
	switch (tst_variant) {
	case 0:
#ifndef HAVE_SIGPENDING
		tst_brk(TCONF, "libc sigpending() is not implemented");
#else
		return sigpending(sigset);
#endif
	break;
	case 1:
		return tst_syscall(__NR_sigpending, sigset);
	case 2:
		return tst_syscall(__NR_rt_sigpending, sigset, SIGSETSIZE);
	}
	return -1;
}

int sighandler_counter;

void sighandler(int signum LTP_ATTRIBUTE_UNUSED)
{
	++sighandler_counter;
}

void test_sigpending(void)
{
	int SIGMAX = MIN(sizeof(sigset_t) * 8, (size_t)_NSIG);
	sighandler_counter = 0;
	sigset_t only_SIGUSR, old_mask;
	sighandler_t old_sighandler1, old_sighandler2;
	sigemptyset(&only_SIGUSR);
	sigaddset(&only_SIGUSR, SIGUSR1);
	sigaddset(&only_SIGUSR, SIGUSR2);
	if (sigprocmask(SIG_SETMASK, &only_SIGUSR, &old_mask))
		tst_brk(TBROK, "sigprocmask failed");
	old_sighandler1 = signal(SIGUSR1, sighandler);
	old_sighandler2 = signal(SIGUSR2, sighandler);
	sigset_t pending;
	sigemptyset(&pending);
tested_sigpending(&pending);
	for (i = 1; i < SIGMAX; ++i)
		if (sigismember(&pending, i))
			tst_brk(TFAIL,
				"initialization failed: no signal should be pending by now");
	if (raise(SIGUSR1))
		tst_brk(TBROK, "raising SIGUSR1 failed");
	if (sighandler_counter > 0)
		tst_brk(TFAIL,
			"signal handler is not (yet) supposed to be called");
	sigemptyset(&pending);
tested_sigpending(&pending);
	for (i = 1; i < SIGMAX; ++i)
		if ((i == SIGUSR1) != sigismember(&pending, i))
			tst_brk(TFAIL, "only SIGUSR1 should be pending by now");
	if (raise(SIGUSR2))
		tst_brk(TBROK, "raising SIGUSR2 failed");
	if (sighandler_counter > 0)
		tst_brk(TFAIL,
			"signal handler is not (yet) supposed to be called");
	sigemptyset(&pending);
tested_sigpending(&pending);
	for (i = 1; i < SIGMAX; ++i)
		if ((i == SIGUSR1 || i == SIGUSR2) != sigismember(&pending, i))
			tst_brk(TFAIL,
				"only SIGUSR1, SIGUSR2 should be pending by now");
	tst_res(TPASS, "basic sigpending test successful");
	if (sigprocmask(SIG_SETMASK, &old_mask, NULL))
		tst_brk(TBROK, "sigprocmask failed");
	if (sighandler_counter != 2)
		tst_brk(TFAIL,
			"signal handler has not been called for each signal");
	signal(SIGUSR1, old_sighandler1);
	signal(SIGUSR2, old_sighandler2);
}

void test_efault_on_invalid_sigset(void)
{
	sigset_t *sigset = tst_get_bad_addr(NULL);
tested_sigpending(sigset);
	if (TST_RET == -1) {
		if (TST_ERR != EFAULT) {
			tst_res(TFAIL | TTERRNO,
				"syscall failed with wrong errno, expected errno=%d, got %d",
				EFAULT, TST_ERR);
		} else {
			tst_res(TPASS | TTERRNO, "expected failure");
		}
	} else {
		tst_res(TFAIL,
			"syscall failed, expected return value=-1, got %ld",
			TST_RET);
	}
}

void run(void)
{
	sigpending_info();
	test_sigpending();
	test_efault_on_invalid_sigset();
}

void main(){
	setup();
	cleanup();
}
