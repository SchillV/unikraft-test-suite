#include "incl.h"

void do_child(int);

void setup(void);

void cleanup(void);
char *TCID = "waitpid05";
int TST_TOTAL = 1;
#ifdef UCLINUX

void do_child_uclinux(void);

int sig_uclinux;
#endif
int main(int ac, char **av)
{
	int pid, npid, sig, nsig;
	int exno, nexno, status;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&do_child_uclinux, "d", &sig_uclinux);
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Set SIGTERM to SIG_DFL as test driver sets up to ignore
		 * SIGTERM
		 */
		if (signal(SIGTERM, SIG_DFL) == SIG_ERR) {
			tst_resm(TFAIL, "Sigset SIGTERM failed, errno = %d",
				 errno);
		}
		exno = 1;
		for (sig = 1; sig <= 15; sig++) {
			if (sig == SIGUSR1 || sig == SIGUSR2 || sig == SIGBUS)
				continue;
			signal(sig, SIG_DFL);
			pid = FORK_OR_VFORK();
			if (pid == 0) {
#ifdef UCLINUX
				self_exec(av[0], "d", sig);
#else
				do_child(sig);
#endif
			} else {
				errno = 0;
				while (((npid = waitpid(pid, &status, 0)) !=
					-1) || (errno == EINTR)) {
					if (errno == EINTR)
						continue;
					if (npid != pid) {
						tst_resm(TFAIL, "waitpid "
							 "error: unexpected "
							 "pid returned");
					} else {
						tst_resm(TPASS, "received "
							 "expected pid.");
					}
					nsig = status % 256;
					 * to check if the core dump bit has
					 * been set, bit #7
					 */
					if (nsig >= 128) {
						nsig -= 128;
						if ((sig == 1) || (sig == 2) ||
						    (sig == 9) || (sig == 13) ||
						    (sig == 14) ||
						    (sig == 15)) {
							tst_resm(TFAIL,
								 "signal "
								 "error : "
								 "core dump "
								 "bit set for"
								 " exception "
								 "number %d",
								 sig);
						}
					} else if ((sig == 3) || (sig == 4) ||
						   (sig == 5) || (sig == 6) ||
						   (sig == 8) || (sig == 11)) {
						tst_resm(TFAIL,
							 "signal error: "
							 "core dump bit not "
							 "set for exception "
							 "number %d", sig);
					}
					 * nsig is the signal number returned
					 * by waitpid
					 */
					if (nsig != sig) {
						tst_resm(TFAIL, "waitpid "
							 "error: unexpected "
							 "signal returned");
						tst_resm(TINFO, "got signal "
							 "%d, expected  "
							 "%d", nsig, sig);
					}
					 * nexno is the exit number returned
					 * by waitpid
					 */
					nexno = status / 256;
					if (nexno != 0) {
						tst_resm(TFAIL, "signal "
							 "error: unexpected "
							 "exit number "
							 "returned");
					} else {
						tst_resm(TPASS, "received "
							 "expected exit number.");
					}
				}
			}
		}
		if (access("core", F_OK) == 0)
			unlink("core");
	}
	cleanup();
	tst_exit();
}

void do_child(int sig)
{
	int exno = 1;
	int pid = getpid();
	if (kill(pid, sig) == -1) {
		tst_resm(TFAIL, "kill error: kill unsuccessful");
		exit(exno);
	}
}
#ifdef UCLINUX

void do_child_uclinux(void)
{
	do_child(sig_uclinux);
}
#endif

void setup(void)
{
	struct rlimit newlimit;
	TEST_PAUSE;
	tst_tmpdir();
	newlimit.rlim_max = newlimit.rlim_cur = RLIM_INFINITY;
	if (setrlimit(RLIMIT_CORE, &newlimit) != 0)
		tst_resm(TWARN,
			 "setrlimit(RLIMIT_CORE,RLIM_INFINITY) failed; this may cause some false core-dump test failures");
}

void cleanup(void)
{
	tst_rmdir();
}

