#include "incl.h"
char *TCID = "pipe06";
int TST_TOTAL = 1;
int pipe_ret, pipes[2];
void setup(void);
void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
pipe(pipes);
		if (TEST_RETURN != -1) {
			tst_resm(TFAIL, "call succeeded unexpectedly");
		}
		if (TEST_ERRNO != EMFILE) {
			tst_resm(TFAIL | TTERRNO, "pipe failed unexpectedly");
		} else {
			tst_resm(TPASS, "failed with EMFILE");
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	int i, numb_fds;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	numb_fds = getdtablesize();
	for (i = 0; i < numb_fds; i++) {
		pipe_ret = pipe(pipes);
		if (pipe_ret < 0) {
			if (errno != EMFILE) {
				tst_brkm(TBROK | TTERRNO, cleanup,
					 "didn't get EMFILE");
			}
			break;
		}
	}
}
void cleanup(void)
{
}

