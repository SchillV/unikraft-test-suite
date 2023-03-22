#include "incl.h"
char *TCID = "pause02";
int TST_TOTAL = 1;

pid_t cpid;

void do_child(void);

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	int status;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&do_child, "");
#endif
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		cpid = FORK_OR_VFORK();
		switch (cpid) {
		case -1:
			tst_brkm(TBROK, cleanup, "fork() failed");
		break;
		case 0:
#ifdef UCLINUX
			if (self_exec(av[0], "") < 0)
				tst_brkm(TBROK, cleanup, "self_exec failed");
#else
			do_child();
#endif
		break;
		default:
		break;
		}
		 * Wait for child to enter pause().
		 */
		TST_PROCESS_STATE_WAIT(cleanup, cpid, 'S');
		 * Send the SIGINT signal now, so that child
		 * returns from pause and resumes execution.
		 */
		kill(cpid, SIGINT);
		wait(&status);
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) == 0)
				tst_resm(TPASS, "pause was interrupted correctly");
			else
				tst_resm(TFAIL, "pause was interrupted but the "
				                "retval and/or errno was wrong");
			continue;
		}
		if (WIFSIGNALED(status)) {
			switch (WTERMSIG(status)) {
			case SIGALRM:
				tst_resm(TFAIL, "Timeout: SIGINT wasn't received by child");
			break;
			default:
				tst_resm(TFAIL, "Child killed by signal");
			}
			continue;
		}
		tst_resm(TFAIL, "Pause was not interrupted");
	}
	cleanup();
	tst_exit();
}

void sig_handle(int sig)
{
}

void do_child(void)
{
	signal(SIGALRM, SIG_DFL);
	if (signal(SIGINT, sig_handle) == SIG_ERR) {
		fprintf(stderr, "Child: Failed to setup signal handler\n");
		exit(1);
	}
	alarm(10);
pause();
	if (TEST_RETURN == -1) {
		if (TEST_ERRNO == EINTR)
			exit(0);
		fprintf(stderr, "Child: Pause returned -1 but errno is %d (%s)\n",
		        TEST_ERRNO, strerror(TEST_ERRNO));
		exit(1);
	}
	fprintf(stderr, "Child: Pause returned %ld\n", TEST_RETURN);
	exit(1);
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
}

