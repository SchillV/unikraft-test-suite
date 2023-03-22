#include "incl.h"
#define DEBUG 0

void do_child(int);

void setup(void);

void cleanup(void);
char *TCID = "waitpid03";
int TST_TOTAL = 1;
#define	MAXUPRC	25

int ikids;

int pid[MAXUPRC];

int condition_number;
#ifdef UCLINUX

void do_child_uclinux(void);

int ikids_uclinux;
#endif
int main(int argc, char **argv)
{
	int lc;
	int status, ret;
	tst_parse_opts(argc, argv, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&do_child, "d", &ikids_uclinux);
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Set SIGTERM to SIG_DFL as test driver sets up to ignore
		 * SIGTERM
		 */
		if ((sig_t) signal(SIGTERM, SIG_DFL) == SIG_ERR) {
			tst_resm(TFAIL, "Signal SIGTERM failed, errno = %d",
				 errno);
		}
		while (++ikids < MAXUPRC) {
			pid[ikids] = FORK_OR_VFORK();
			if (pid[ikids] > 0) {
				if (DEBUG)
					tst_resm(TINFO, "child # %d", ikids);
			} else if (pid[ikids] == -1) {
				tst_brkm(TBROK|TERRNO, cleanup, "cannot open "
					"fork #%d", ikids);
			} else {
#ifdef UCLINUX
				if (self_exec(argv[0], "d", ikids) < 0) {
					tst_resm(TFAIL, "cannot self_exec #%d",
						 ikids);
				}
#else
				do_child(ikids);
#endif
			}
		}
		for (ikids = 1; ikids < MAXUPRC; ikids++) {
			if (DEBUG)
				tst_resm(TINFO, "Killing #%d", ikids);
			kill(pid[ikids], SIGTERM);
		}
		ikids = 0;
		condition_number = 1;
		if (DEBUG)
			tst_resm(TINFO, "Waiting for child:#%d", MAXUPRC / 2);
		ret = waitpid(pid[MAXUPRC / 2], &status, 0);
		if (ret != pid[MAXUPRC / 2]) {
			tst_resm(TFAIL, "condition %d test failed. "
				 "waitpid(%d) returned %d.",
				 condition_number, pid[MAXUPRC / 2], ret);
		} else {
			tst_resm(TPASS, "Got correct child PID");
		}
		condition_number++;
		 * Child has already been waited on, waitpid should return
		 * -1
		 */
		ret = waitpid(pid[MAXUPRC / 2], &status, 0);
		if (ret != -1) {
			tst_resm(TFAIL, "condition %d test failed",
				 condition_number);
		} else {
			tst_resm(TPASS, "Condition %d test passed",
				 condition_number);
		}
		condition_number++;
	}
	cleanup();
	tst_exit();
}

void do_child(int ikids)
{
	if (DEBUG)
		tst_resm(TINFO, "child:%d", ikids);
	pause();
	exit(0);
}
#ifdef UCLINUX

void do_child_uclinux(void)
{
	do_child(ikids_uclinux);
}
#endif

void setup(void)
{
	TEST_PAUSE;
}

void cleanup(void)
{
	while (ikids-- > 1)
		kill(pid[ikids], SIGKILL);
}

