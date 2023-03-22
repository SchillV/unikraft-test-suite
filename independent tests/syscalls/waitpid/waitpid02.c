#include "incl.h"

void do_child(void);

void setup(void);
char *TCID = "waitpid02";
int TST_TOTAL = 1;
int main(int argc, char **argv)
{
	int lc;
	int pid, npid, sig, nsig;
	int nexno, status;
	tst_parse_opts(argc, argv, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&do_child, "");
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		sig = SIGFPE;
		pid = FORK_OR_VFORK();
		if (pid < 0)
			tst_brkm(TBROK|TERRNO, NULL, "fork failed");
		if (pid == 0) {
#ifdef UCLINUX
			self_exec(argv[0], "");
#else
			do_child();
#endif
		} else {
			kill(pid, sig);
			errno = 0;
			while (((npid = waitpid(pid, &status, 0)) != -1) ||
			       (errno == EINTR)) {
				if (errno == EINTR)
					continue;
				if (npid != pid) {
					tst_resm(TFAIL, "waitpid error: "
						 "unexpected pid returned");
				} else {
					tst_resm(TPASS,
						 "received expected pid");
				}
				nsig = WTERMSIG(status);
				 * nsig is the signal number returned by
				 * waitpid
				 */
				if (nsig != sig) {
					tst_resm(TFAIL, "waitpid error: "
						 "unexpected signal returned");
				} else {
					tst_resm(TPASS, "received expected "
						 "signal");
				}
				 * nexno is the exit number returned by
				 * waitpid
				 */
				nexno = WEXITSTATUS(status);
				if (nexno != 0) {
					tst_resm(TFAIL, "signal error: "
						 "unexpected exit number "
						 "returned");
				} else {
					tst_resm(TPASS, "received expected "
						 "exit value");
				}
			}
		}
	}
	tst_exit();
}

void do_child(void)
{
	int exno = 1;
	while (1)
		usleep(10);
	exit(exno);
}

void setup(void)
{
	 * '1' is a special value, that will also avoid dumping via pipe. */
	struct rlimit r;
	r.rlim_cur = 1;
	r.rlim_max = 1;
	setrlimit(RLIMIT_CORE, &r);
	TEST_PAUSE;
}

