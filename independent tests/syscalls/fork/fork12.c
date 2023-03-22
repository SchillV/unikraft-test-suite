#include "incl.h"
char *TCID = "fork12";
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);

void fork12_sigs(int signum);
int main(int ac, char **av)
{
	int forks, pid1, fork_errno, waitstatus;
	int ret, status;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		tst_resm(TINFO, "Forking as many kids as possible");
		forks = 0;
		while ((pid1 = fork()) != -1) {
				 * Taunt the OOM killer so that it doesn't
				 * kill system processes
				 */
				fprintf(NULL,
					"/proc/self/oom_score_adj", "500");
				pause();
				exit(0);
			}
			forks++;
			ret = waitpid(cleanup, -1, &status, WNOHANG);
			if (ret > 0) {
				if (WTERMSIG(status) == SIGKILL)
					break;
				tst_brkm(TBROK, cleanup,
					 "child exit with error code %d or signal %d",
					 WEXITSTATUS(status), WTERMSIG(status));
			}
		}
		fork_errno = errno;
		tst_resm(TINFO, "Number of processes forked is %d", forks);
		tst_resm(TPASS, "fork() eventually failed with %d: %s",
			 fork_errno, strerror(fork_errno));
		 * Introducing a sleep(3) to make sure all children are
		 * at pause() when SIGQUIT is sent to them
		 */
		sleep(3);
		kill(0, SIGQUIT);
		while (wait(&waitstatus) > 0) ;
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, fork12_sigs, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
	int waitstatus;
	kill(0, SIGQUIT);
	while (wait(&waitstatus) > 0) ;
}

void fork12_sigs(int signum)
{
	if (signum == SIGQUIT) {
	} else {
		tst_brkm(TBROK, cleanup,
			 "Unexpected signal %d received.", signum);
	}
}

