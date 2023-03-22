#include "incl.h"
char *TCID = "setpgrp02";
int TST_TOTAL = 1;
void setup(void);
void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	int pid, oldpgrp;
	int e_code, status, retval = 0;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		if ((pid = FORK_OR_VFORK()) == -1) {
			tst_brkm(TBROK, cleanup, "fork() failed");
		}
			oldpgrp = getpgrp();
setpgrp();
			if (TEST_RETURN != 0) {
				retval = 1;
				tst_resm(TFAIL, "setpgrp() FAILED, errno:%d",
					 errno);
				continue;
			}
			if (getpgrp() == oldpgrp) {
				retval = 1;
				tst_resm(TFAIL, "setpgrp() FAILED to set "
					 "new group id");
				continue;
			} else {
				tst_resm(TPASS, "functionality is correct");
			}
			exit(retval);
			wait(&status);
			e_code = status >> 8;
			if ((e_code != 0) || (retval != 0)) {
				tst_resm(TFAIL, "Failures reported above");
			}
			cleanup();
		}
	}
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

