#include "incl.h"
char *TCID = "ptrace05";
int TST_TOTAL = 0;
int usage(const char *);
int usage(const char *argv0)
{
	fprintf(stderr, "usage: %s [start-signum] [end-signum]\n", argv0);
	return 1;
}
int main(int argc, char **argv)
{
	int end_signum = -1;
	int signum;
	int start_signum = -1;
	int status;
	pid_t child;
	tst_parse_opts(argc, argv, NULL, NULL);
	if (start_signum == -1) {
		start_signum = 0;
	}
	if (end_signum == -1) {
		end_signum = SIGRTMAX;
	}
	for (signum = start_signum; signum <= end_signum; signum++) {
		if (signum >= __SIGRTMIN && signum < SIGRTMIN)
			continue;
		switch (child = fork()) {
		case -1:
			tst_brkm(TBROK | TERRNO, NULL, "fork() failed");
		case 0:
			if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) != -1) {
				tst_resm(TINFO, "[child] Sending kill(.., %d)",
					 signum);
				if (kill(getpid(), signum) < 0) {
					tst_resm(TINFO | TERRNO,
						 "[child] kill(.., %d) failed.",
						 signum);
				}
			} else {
				 * This won't increment the TST_COUNT var.
				 * properly, but it'll show up as a failure
				 * nonetheless.
				 */
				tst_resm(TFAIL | TERRNO,
					 "Failed to ptrace(PTRACE_TRACEME, ...) "
					 "properly");
			}
			exit((signum == 0 ? 0 : 2));
			break;
		default:
			waitpid(child, &status, 0);
			switch (signum) {
			case 0:
				if (WIFEXITED(status)
				    && WEXITSTATUS(status) == 0) {
					tst_resm(TPASS,
						 "kill(.., 0) exited "
						 "with 0, as expected.");
				} else {
					tst_resm(TFAIL,
						 "kill(.., 0) didn't exit "
						 "with 0.");
				}
				break;
			case SIGKILL:
				if (WIFSIGNALED(status)) {
					if (WTERMSIG(status) == SIGKILL) {
						tst_resm(TPASS,
							 "Killed with SIGKILL, "
							 "as expected.");
					} else {
						tst_resm(TPASS,
							 "Didn't die with "
							 "SIGKILL (?!) ");
					}
				} else if (WIFEXITED(status)) {
					tst_resm(TFAIL,
						 "Exited unexpectedly instead "
						 "of dying with SIGKILL.");
				} else if (WIFSTOPPED(status)) {
					tst_resm(TFAIL,
						 "Stopped instead of dying "
						 "with SIGKILL.");
				}
				break;
			default:
				if (WIFSTOPPED(status)) {
					tst_resm(TPASS, "Stopped as expected");
				} else {
					tst_resm(TFAIL, "Didn't stop as "
						 "expected.");
					if (kill(child, 0)) {
						tst_resm(TINFO,
							 "Is still alive!?");
					} else if (WIFEXITED(status)) {
						tst_resm(TINFO,
							 "Exited normally");
					} else if (WIFSIGNALED(status)) {
						tst_resm(TINFO,
							 "Was signaled with "
							 "signum=%d",
							 WTERMSIG(status));
					}
				}
				break;
			}
		}
		kill(child, 9);
	}
	tst_exit();
}

