#include "incl.h"
#define TESTUSER	"nobody"
TCID_DEFINE(setgroups02);
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Call setgroups() to set supplimentary group IDs of
		 * the calling super-user process to gid of TESTUSER.
		 */
SETGROUPS(cleanup, gidsetsize, groups_list);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL, "setgroups(%d, groups_list) Failed, "
				 "errno=%d : %s", gidsetsize, TEST_ERRNO,
				 strerror(TEST_ERRNO));
			continue;
		}
int 		 * Call getgroups(2) to verify that
		 * setgroups(2) successfully set the
		 * supp. gids of TESTUSER.
		 */
		groups_list[0] = '\0';
		if (GETGROUPS(cleanup, gidsetsize, groups_list) < 0) {
			tst_brkm(TFAIL, cleanup, "getgroups() Fails, "
				 "error=%d", errno);
		}
		for (i = 0; i < NGROUPS; i++) {
			if (groups_list[i] == user_info->pw_gid) {
				tst_resm(TPASS,
					 "Functionality of setgroups"
					 "(%d, groups_list) successful",
					 gidsetsize);
				PASS_FLAG = 1;
			}
		}
		if (PASS_FLAG == 0) {
			tst_resm(TFAIL, "Supplimentary gid %d not set "
				 "for the process", user_info->pw_gid);
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
	if ((user_info = getpwnam(TESTUSER)) == NULL) {
		tst_brkm(TFAIL, cleanup, "getpwnam(2) of %s Failed", TESTUSER);
	}
	if (!GID_SIZE_CHECK(user_info->pw_gid)) {
		tst_brkm(TBROK,
			 cleanup,
			 "gid returned from getpwnam is too large for testing setgroups16");
	}
	groups_list[0] = user_info->pw_gid;
}
void cleanup(void)
{
}

