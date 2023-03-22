#include "incl.h"
char *TCID = "fork03";
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	float fl1, fl2;
	int i;
	int pid1, pid2, status;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		pid1 = fork();
		if (pid1 == -1)
			tst_brkm(TBROK, cleanup, "fork() failed");
		if (pid1 == 0) {
			for (i = 1; i < 32767; i++) {
				fl1 = 0.000001;
				fl1 = fl2 = 0.000001;
				fl1 = fl1 * 10.0;
				fl2 = fl1 / 1.232323;
				fl1 = fl2 - fl2;
				fl1 = fl2;
			}
			if (pid1 != 0)
				exit(1);
			else
				exit(0);
		} else {
			tst_resm(TINFO, "process id in parent of child from "
				 "fork : %d", pid1);
			if (pid1 != pid2) {
				tst_resm(TFAIL, "pids don't match : %d vs %d",
					 pid1, pid2);
				continue;
			}
			if ((status >> 8) != 0) {
				tst_resm(TFAIL, "child exited with failure");
				continue;
			}
			tst_resm(TPASS, "test 1 PASSED");
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
}

