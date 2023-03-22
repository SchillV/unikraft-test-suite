#include "incl.h"

void cleanup(void);

void setup(void);
char *TCID = "exit01";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int pid, npid, sig, nsig, exno, nexno, status;
	int rval = 0;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		sig = 0;
		exno = 1;
		pid = FORK_OR_VFORK();
		switch (pid) {
		case 0:
			exit(exno);
		break;
		case -1:
			tst_brkm(TBROK | TERRNO, cleanup, "fork() failed");
		break;
		default:
			npid = wait(&status);
			if (npid != pid) {
				tst_resm(TFAIL, "wait error: "
					 "unexpected pid returned");
				rval = 1;
			}
			nsig = status % 256;
			 * Check if the core dump bit has been set, bit # 7
			 */
			if (nsig >= 128) {
				nsig = nsig - 128;
			}
			 * nsig is the signal number returned by wait
			 */
			if (nsig != sig) {
				tst_resm(TFAIL, "wait error: "
					 "unexpected signal returned");
				rval = 1;
			}
			 * nexno is the exit number returned by wait
			 */
			nexno = status / 256;
			if (nexno != exno) {
				tst_resm(TFAIL, "wait error: "
					 "unexpected exit number returned");
				rval = 1;
			}
		break;
		}
		if (rval != 1) {
			tst_resm(TPASS, "exit() test PASSED");
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

