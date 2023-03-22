#include "incl.h"
char *TCID = "getresuid03";
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
		 * user id's of the calling process after
		 * setreuid() in setup.
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
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	pr_uid = getuid();
	ps_uid = geteuid();
	if ((user_id = getpwnam("nobody")) == NULL) {
		tst_brkm(TBROK, cleanup,
			 "getpwnam(nobody) Failed, errno=%d", errno);
	}
	pe_uid = user_id->pw_uid;
	 * Set the effective user-id of the process to that of
	 * test user "ltpuser1".
	 * The real/saved  user id remains same as of caller.
	 */
	if (setresuid(-1, pe_uid, -1) < 0) {
		tst_brkm(TBROK, cleanup,
			 "setresuid(-1, %d, -1) Fails, errno:%d : %s",
			 ps_uid, errno, strerror(errno));
	}
}
void cleanup(void)
{
	if (setreuid(-1, pr_uid) < 0) {
		tst_brkm(TBROK, NULL, "resetting process effective uid failed");
	}
}

