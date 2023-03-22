#include "incl.h"
void cleanup(void);
void setup(void);
void do_child(void);
char *TCID = "kill08";
int TST_TOTAL = 1;
#define TEST_SIG SIGKILL
int main(int ac, char **av)
{
	int lc;
	pid_t pid1, pid2;
	int exno, status, nsig, i;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&do_child, "");
#endif
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		status = 1;
		exno = 1;
		pid1 = FORK_OR_VFORK();
		if (pid1 < 0) {
			tst_brkm(TBROK, cleanup, "Fork of first child failed");
		} else if (pid1 == 0) {
			setpgrp();
			for (i = 0; i < 5; i++) {
				pid2 = FORK_OR_VFORK();
				if (pid2 < 0) {
					tst_brkm(TBROK, cleanup, "Fork failed");
				} else if (pid2 == 0) {
#ifdef UCLINUX
					if (self_exec(av[0], "") < 0) {
						tst_brkm(TBROK, cleanup,
							 "self_exec of "
							 "child failed");
					}
#else
					do_child();
#endif
				}
			}
kill(0, TEST_SIG);
			pause();
			exit(exno);
		} else {
			waitpid(pid1, &status, 0);
			if (TEST_RETURN != 0) {
				tst_brkm(TFAIL, cleanup, "%s failed - errno = "
					 "%d : %s", TCID, TEST_ERRNO,
					 strerror(TEST_ERRNO));
			}
		}
		 * Check to see if the process was terminated with the
		 * expected signal.
		 */
		nsig = WTERMSIG(status);
		if (nsig == TEST_SIG) {
			tst_resm(TPASS, "received expected signal %d",
				 nsig);
		} else {
			tst_resm(TFAIL,
				 "expected signal %d received %d",
				 TEST_SIG, nsig);
		}
	}
	cleanup();
	tst_exit();
}
void do_child(void)
{
	int exno = 1;
	pause();
	exit(exno);
}
void setup(void)
{
	TEST_PAUSE;
}
void cleanup(void)
{
}

