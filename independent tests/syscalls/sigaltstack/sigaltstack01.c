#include "incl.h"
char *TCID = "sigaltstack01";
int TST_TOTAL = 1;
int got_signal = 0;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		sigstk.ss_size = SIGSTKSZ;
		sigstk.ss_flags = 0;
sigaltstack(&sigstk, &osigstk);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL,
				 "sigaltstack() Failed, errno=%d : %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
		} else {
			act.sa_flags = SA_ONSTACK;
			act.sa_handler = (void (*)())sig_handler;
			if ((sigaction(SIGUSR1, &act, &oact)) == -1) {
				tst_brkm(TFAIL, cleanup, "sigaction() "
					 "fails to trap signal "
					 "delivered on alt. stack, "
					 "error=%d", errno);
			}
			kill(my_pid, SIGUSR1);
			while (!got_signal) ;
			got_signal = 0;
			alt_stk = addr;
			 * First,
			 * Check that alt_stk is within the
			 * alternate stk boundaries
			 *
			 * Second,
			 * Check that main_stk is outside the
			 * alternate stk boundaries.
			 */
			if ((alt_stk < sigstk.ss_sp) &&
			    (alt_stk > (sigstk.ss_sp + SIGSTKSZ))) {
				tst_resm(TFAIL,
					 "alt. stack is not within the "
					 "alternate stk boundaries");
			} else if ((main_stk >= sigstk.ss_sp) &&
				   (main_stk <=
				    (sigstk.ss_sp + SIGSTKSZ))) {
				tst_resm(TFAIL,
					 "main stk. not outside the "
					 "alt. stack boundaries");
			} else {
				tst_resm(TPASS,
					 "Functionality of "
					 "sigaltstack() successful");
			}
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	my_pid = getpid();
	act.sa_handler = (void (*)(int))sig_handler;
	if ((sigaction(SIGUSR1, &act, &oact)) == -1) {
		tst_brkm(TFAIL, cleanup,
			 "sigaction() fails in setup, errno=%d", errno);
	}
	kill(my_pid, SIGUSR1);
	while (!got_signal) ;
	got_signal = 0;
	main_stk = addr;
	if ((sigstk.ss_sp = malloc(SIGSTKSZ)) == NULL) {
		tst_brkm(TFAIL, cleanup,
			 "could not allocate memory for the alternate stack");
	}
}
void sig_handler(int n)
{
	int i;
	(void) n;
	addr = &i;
	got_signal = 1;
}
void cleanup(void)
{
	free(sigstk.ss_sp);
}

