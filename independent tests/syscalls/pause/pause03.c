#include "incl.h"

pid_t cpid;
char *TCID = "pause03";
int TST_TOTAL = 1;

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
		if ((cpid = FORK_OR_VFORK()) == -1)
			tst_brkm(TBROK | TERRNO, NULL, "fork() failed");
		if (cpid == 0) {
#ifdef UCLINUX
			if (self_exec(av[0], "") < 0)
				tst_brkm(TBROK, cleanup, "self_exec failed");
#else
			do_child();
#endif
		}
		TST_PROCESS_STATE_WAIT(cleanup, cpid, 'S');
		kill(cpid, SIGKILL);
		wait(NULL, &status);
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
			tst_resm(TPASS, "pause() did not return after SIGKILL");
			continue;
		}
		if (WIFSIGNALED(status)) {
			tst_resm(TFAIL, "child killed by %s unexpectedly",
			         tst_strsig(WTERMSIG(status)));
			continue;
		}
		tst_resm(TFAIL, "child exited with %i", WEXITSTATUS(status));
	}
	cleanup();
	tst_exit();
}
void do_child(void)
{
pause();
	tst_resm(TFAIL, "Unexpected return from pause()");
	exit(0);
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
	kill(cpid, SIGKILL);
}

