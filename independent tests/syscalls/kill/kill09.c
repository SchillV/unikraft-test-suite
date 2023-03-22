#include "incl.h"
void setup();
void cleanup();
void alarm_handler(int sig);
void do_child();
char *TCID = "kill09";
int TST_TOTAL = 1;
int fork_pid;
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
		if ((fork_pid = FORK_OR_VFORK()) == -1)
			tst_brkm(TBROK | TERRNO, cleanup, "fork failed");
		if (fork_pid == 0) {
#ifdef UCLINUX
			if (self_exec(av[0], "") < 0) {
				tst_brkm(TBROK, cleanup,
					 "self_exec of child failed");
			}
#else
			do_child();
#endif
		}
kill(fork_pid, SIGKILL);
		if (TEST_RETURN == -1)
			tst_resm(TFAIL | TTERRNO, "kill(.., SIGKILL) failed");
		else {
			tst_resm(TPASS,
				 "kill(%d, SIGKILL) returned %ld",
				 fork_pid, TEST_RETURN);
		}
		waitpid(0, &status, WNOHANG);
	}
	cleanup();
	tst_exit();
}
void do_child(void)
{
	 * Setup alarm signal if we don't get the signal to prevent this process
	 * from hanging around forever.
	 */
	signal(SIGALRM, alarm_handler);
	alarm(20);
	pause();
	exit(1);
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	(void)signal(SIGCHLD, SIG_IGN);
	TEST_PAUSE;
}
void cleanup(void)
{
}
void alarm_handler(int sig)
{
	exit(8);
}

