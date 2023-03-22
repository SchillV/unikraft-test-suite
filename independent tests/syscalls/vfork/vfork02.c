#include "incl.h"
#define _GNU_SOURCE 1
char *TCID = "vfork02";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Call vfork(2) to create a child process without
		 * fully copying the address space of parent.
		 */
vfork();
		if ((cpid = TEST_RETURN) == -1) {
			tst_resm(TFAIL, "vfork() Failed, errno=%d : %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
			 * Check whether the pending signal SIGUSR1
			 * in the parent is also pending in the child
			 * process by storing it in a variable.
			 */
			if (sigpending(&PendSig) == -1) {
				tst_resm(TFAIL, "sigpending function "
					 "failed in child");
				_exit(1);
			}
			if (sigismember(&PendSig, SIGUSR1) != 0) {
				tst_resm(TFAIL, "SIGUSR1 also pending "
					 "in child process");
				_exit(1);
			}
			 * Exit with normal exit code if everything
			 * fine
			 */
			_exit(0);
			 * Let the parent process wait till child completes
			 * its execution.
			 */
			wait(&exit_status);
			if (WEXITSTATUS(exit_status) == 0) {
				tst_resm(TPASS, "Call to vfork() "
					 "successful");
			} else if (WEXITSTATUS(exit_status) == 1) {
				tst_resm(TFAIL,
					 "Child process exited abnormally");
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
	if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
		tst_brkm(TBROK, cleanup, "Fails to catch the signal SIGUSR1");
	}
	if (sighold(SIGUSR1) == -1) {
		tst_brkm(TBROK, cleanup,
			 "sighold failed to hold the signal SIGUSR1");
	}
	kill(cleanup, getpidcleanup, getpid), SIGUSR1);
	if (sigpending(&PendSig) == -1) {
		tst_brkm(TBROK, cleanup,
			 "sigpending function failed in parent");
	}
	if (sigismember(&PendSig, SIGUSR1) != 1) {
		tst_brkm(TBROK, cleanup,
			 "SIGUSR1 signal is not pending in parent");
	}
}
void sig_handler(void)
{
}
void cleanup(void)
{
	if (sigrelse(SIGUSR1) == -1) {
		tst_brkm(TBROK, NULL, "Failed to release 'SIGUSR1' in cleanup");
	}
}

