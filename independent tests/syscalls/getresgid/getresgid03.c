#include "incl.h"
#define _GNU_SOURCE 1
char *TCID = "getresgid03";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	 eff_gid, sav_gid;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Call getresgid() to get the real/effective/saved
		 * user id's of the calling process after
		 * setregid() in setup.
		 */
GETRESGID(cleanup, &real_gid, &eff_gid, &sav_gid);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL, "getresgid() Failed, errno=%d : %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
			continue;
		}
		 * Verify the real/effective/saved gid
		 * values returned by getresgid with the
		 * expected values.
		 */
		if ((real_gid != pr_gid) || (eff_gid != pe_gid) ||
		    (sav_gid != ps_gid)) {
			tst_resm(TFAIL, "real:%d, effective:%d, "
				 "saved-user:%d ids differ",
				 real_gid, eff_gid, sav_gid);
		} else {
			tst_resm(TPASS, "Functionality of getresgid() "
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
	pr_gid = getgid();
	ps_gid = getegid();
	if ((user_id = getpwnam("nobody")) == NULL) {
		tst_brkm(TBROK, cleanup,
			 "getpwnam(nobody) Failed, errno=%d", errno);
	}
	pe_gid = user_id->pw_gid;
	 * Set the effective user-id of the process to that of
	 * test user "ltpuser1".
	 * The real/saved  user id remains same as of caller.
	 */
	if (setresgid(-1, pe_gid, -1) < 0) {
		tst_brkm(TBROK, cleanup,
			 "setresgid(-1, %d, -1) Fails, errno:%d : %s",
			 ps_gid, errno, strerror(errno));
	}
}
void cleanup(void)
{
	if (setregid(-1, pr_gid) < 0) {
		tst_brkm(TBROK, NULL, "resetting process effective gid failed");
	}
}

