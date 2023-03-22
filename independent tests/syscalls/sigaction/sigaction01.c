#include "incl.h"
void setup();
void cleanup();
char *TCID = "sigaction01";
int TST_TOTAL = 4;
volatile sig_atomic_t testcase_no;
volatile sig_atomic_t pass;
void handler(int sig, siginfo_t * sip, void *ucp)
{
	struct sigaction oact;
	int err;
	sigset_t nmask, omask;
	 * Get sigaction setting
	 */
	err = sigaction(SIGUSR1, NULL, &oact);
	if (err == -1) {
		perror("sigaction");
		return;
	}
	 * Get current signal mask
	 */
	sigemptyset(&nmask);
	sigemptyset(&omask);
	err = sigprocmask(SIG_BLOCK, &nmask, &omask);
	if (err == -1) {
		perror("sigprocmask");
		tst_resm(TWARN, "sigprocmask() failed");
		return;
	}
	switch (testcase_no) {
	case 1:
		 * SA_RESETHAND and SA_SIGINFO were set. SA_SIGINFO should
		 * be clear in Linux. In Linux kernel, SA_SIGINFO is not
		 * cleared in psig().
		 */
		if (!(oact.sa_flags & SA_SIGINFO)) {
			tst_resm(TFAIL, "SA_RESETHAND should not "
				 "cause SA_SIGINFO to be cleared, but it was.");
			return;
		}
		if (sip == NULL) {
			tst_resm(TFAIL, "siginfo should not be NULL");
			return;
		}
		tst_resm(TPASS, "SA_RESETHAND did not "
			 "cause SA_SIGINFO to be cleared");
		break;
	case 2:
		 * In Linux, SA_RESETHAND doesn't imply SA_NODEFER; sig
		 * should not be masked.  The testcase should pass if
		 * SA_NODEFER is not masked, ie. if SA_NODEFER is a member
		 * of the signal list
		 */
		if (sigismember(&omask, sig) == 0) {
			tst_resm(TFAIL, "SA_RESETHAND should cause sig to "
				 "be masked when the handler executes.");
			return;
		}
		tst_resm(TPASS, "SA_RESETHAND was masked when handler "
			 "executed");
		break;
	case 3:
		 * SA_RESETHAND implies SA_NODEFER unless sa_mask already
		 * included sig.
		 */
		if (!sigismember(&omask, sig)) {
			tst_resm(TFAIL, "sig should continue to be masked "
				 "because sa_mask originally contained sig.");
			return;
		}
		tst_resm(TPASS, "sig has been masked "
			 "because sa_mask originally contained sig");
		break;
	case 4:
		 * A signal generated from a mechanism that does not provide
		 * siginfo should invoke the handler with a non-NULL siginfo
		 * pointer.
		 */
		if (sip == NULL) {
			tst_resm(TFAIL, "siginfo pointer should not be NULL");
			return;
		}
		tst_resm(TPASS, "siginfo pointer non NULL");
		break;
	default:
		tst_resm(TFAIL, "invalid test case number: %d", testcase_no);
		exit(1);
	}
}
int set_handler(int flags, int sig_to_mask)
{
	struct sigaction sa;
	sa.sa_sigaction = handler;
	sa.sa_flags = flags;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, sig_to_mask);
sigaction(SIGUSR1, &sa, NULL);
	if (TEST_RETURN != 0) {
		perror("sigaction");
		tst_resm(TFAIL, "call failed unexpectedly");
		return 1;
	}
	return 0;
}
void setup(void)
{
	TEST_PAUSE;
}
void cleanup(void)
{
}
int main(int ac, char **av)
{
	int lc;
	int i;
	int test_flags[] = { SA_RESETHAND | SA_SIGINFO, SA_RESETHAND,
		SA_RESETHAND | SA_SIGINFO, SA_RESETHAND | SA_SIGINFO
	};
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		testcase_no = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			if (set_handler(test_flags[i], 0) == 0) {
				testcase_no++;
				switch (i) {
				case 0:
					(void)kill(getpid(), SIGUSR1);
					break;
				case 2:
					(void)
					    pthread_kill(pthread_self(),
							 SIGUSR1);
					break;
				default:
					tst_brkm(TBROK, cleanup,
						 "illegal case number");
					break;
				}
			}
		}
	}
	cleanup();
	tst_exit();
}

