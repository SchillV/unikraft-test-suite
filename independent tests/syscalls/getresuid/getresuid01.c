#include "incl.h"
char *TCID = "getresuid01";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	 eff_uid, sav_uid;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Call getresuid() to get the real/effective/saved
		 * user id's of the calling process.
		 */
GETRESUID(cleanup, &real_uid, &eff_uid, &sav_uid);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL, "getresuid() Failed, errno=%d : %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
			continue;
		}
		if ((real_uid != pr_uid) || (eff_uid != pe_uid) ||
		    (sav_uid != ps_uid)) {
			tst_resm(TFAIL, "real:%d, effective:%d, "
				 "saved-user:%d ids differ",
				 real_uid, eff_uid, sav_uid);
		} else {
			tst_resm(TPASS, "Functionality of getresuid() "
				 "successful");
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	pr_uid = getuid();
	pe_uid = geteuid();
	ps_uid = geteuid();
}
void cleanup(void)
{
}

