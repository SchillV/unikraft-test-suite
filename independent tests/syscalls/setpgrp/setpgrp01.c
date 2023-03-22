#include "incl.h"
void setup();
void cleanup();
char *TCID = "setpgrp01";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * TEST CASE:
		 *  Call the setpgrp system call
		 */
setpgrp();
		if (TEST_RETURN != 0) {
			tst_resm(TFAIL,
				 "setpgrp -  Call the setpgrp system call failed, errno=%d : %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
		} else {
			tst_resm(TPASS,
				 "setpgrp -  Call the setpgrp system call returned %ld",
				 TEST_RETURN);
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	int pid, status;
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	 * Make sure current process is NOT a session or pgrp leader
	 */
	if (getpgrp() == getpid()) {
		if ((pid = FORK_OR_VFORK()) == -1) {
			tst_brkm(TBROK, cleanup,
				 "fork() in setup() failed - errno %d", errno);
		}
			wait(&status);
			exit(WEXITSTATUS(status));
		}
	}
}
void cleanup(void)
{
}

