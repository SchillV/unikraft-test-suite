#include "incl.h"
char *TCID = "pipe08";
int TST_TOTAL = 1;
void setup(void);
void cleanup(void);
void sighandler(int);
int main(int ac, char **av)
{
	int lc;
	char wrbuf[BUFSIZ];
	int written, length;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
pipe(pipefd);
		if (TEST_RETURN != 0) {
			tst_resm(TFAIL, "call failed unexpectedly");
			continue;
		}
		if ((close_stat = close(pipefd[0])) == -1) {
			tst_brkm(TBROK, cleanup, "close of read side failed");
		}
		strcpy(wrbuf, "abcdefghijklmnopqrstuvwxyz\0");
		length = strlen(wrbuf);
		 * the SIGPIPE signal will be caught here or else
		 * the program will dump core when the signal is
		 * sent
		 */
		written = write(pipefd[1], wrbuf, length);
		if (written > 0)
			tst_brkm(TBROK, cleanup, "write succeeded unexpectedly");
	}
	cleanup();
	tst_exit();
}
void sighandler(int sig)
{
	if (sig != SIGPIPE)
		tst_resm(TFAIL, "expected SIGPIPE, got %d", sig);
	else
		tst_resm(TPASS, "got expected SIGPIPE signal");
}
void setup(void)
{
	tst_sig(NOFORK, sighandler, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}

