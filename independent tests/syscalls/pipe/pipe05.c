#include "incl.h"
char *TCID = "pipe05";
int TST_TOTAL = 1;
intptr_t pipes;
void setup(void);
void cleanup(void);
jmp_buf sig11_recover;
void sig11_handler(int sig);
int main(int ac, char **av)
{
	volatile int lc;
	struct sigaction sa, osa;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		sa.sa_handler = &sig11_handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sigaction(SIGSEGV, NULL, &osa);
		sigaction(SIGSEGV, &sa, NULL);
		if (setjmp(sig11_recover)) {
			TEST_RETURN = -1;
			TEST_ERRNO = EFAULT;
		} else {
pipe((int *)pipes);
		}
		sigaction(SIGSEGV, &osa, NULL);
		if (TEST_RETURN != -1) {
			tst_resm(TFAIL, "call succeeded unexpectedly");
		}
		if (TEST_ERRNO != EFAULT) {
			tst_resm(TFAIL, "unexpected error - %d : %s - "
				 "expected EMFILE", TEST_ERRNO,
				 strerror(TEST_ERRNO));
		} else {
			tst_resm(TPASS, "expected failure - "
				 "errno = %d : %s", TEST_ERRNO,
				 strerror(TEST_ERRNO));
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void sig11_handler(int sig LTP_ATTRIBUTE_UNUSED)
{
	longjmp(sig11_recover, 1);
}
void cleanup(void)
{
}

