#include "incl.h"
void setup();
void cleanup();
TCID_DEFINE(setgroups01);
int TST_TOTAL = 1;
int len = NGROUPS, ngrps = 0;
GID_T list[NGROUPS];
int main(int ac, char **av)
{
	int lc;
     * parse standard options
     ***************************************************************/
	tst_parse_opts(ac, av, NULL, NULL);
     * perform global setup for test
     ***************************************************************/
	setup();
     * check looping state if -c option given
     ***************************************************************/
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Call setgroups(2)
		 */
SETGROUPS(cleanup, ngrps, list);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL,
				 "setgroups(%d, list) Failed, errno=%d : %s",
				 len, TEST_ERRNO, strerror(TEST_ERRNO));
		} else {
			tst_resm(TPASS,
				 "setgroups(%d, list) returned %ld",
				 len, TEST_RETURN);
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	ngrps = GETGROUPS(cleanup, len, list);
	if (ngrps == -1) {
		tst_brkm(TBROK, cleanup,
			 "getgroups(%d, list) Failure. errno=%d : %s",
			 len, errno, strerror(errno));
	}
}
void cleanup(void)
{
}

