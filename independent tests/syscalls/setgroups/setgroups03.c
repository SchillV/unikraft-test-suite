#include "incl.h"
#define TESTUSER	"nobody"
char nobody_uid[] = "nobody";
struct passwd *ltpuser;
TCID_DEFINE(setgroups03);
int TST_TOTAL = 2;
	size_t gsize_add;
	int list;
	char *desc;
	int exp_errno;
	int (*setupfunc) ();
} Test_cases[] = {
	{
	1, 1, "Size is > sysconf(_SC_NGROUPS_MAX)", EINVAL, NULL}, {
	0, 2, "Permission denied, not super-user", EPERM, setup1}
};
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	groups_list = malloc(ngroups_max * sizeof(GID_T));
	if (groups_list == NULL) {
		tst_brkm(TBROK, NULL, "malloc failed to alloc %zu errno "
			 " %d ", ngroups_max * sizeof(GID_T), errno);
	}
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			if (Test_cases[i].setupfunc != NULL) {
				Test_cases[i].setupfunc();
			}
			gidsetsize = ngroups_max + Test_cases[i].gsize_add;
			test_desc = Test_cases[i].desc;
			 * Call setgroups() to test different test conditions
int 			 * verify that it fails with -1 return value and
			 * sets appropriate errno.
			 */
SETGROUPS(cleanup, gidsetsize, groups_list);
			if (TEST_RETURN != -1) {
				tst_resm(TFAIL, "setgroups(%d) returned %ld, "
					 "expected -1, errno=%d", gidsetsize,
					 TEST_RETURN, Test_cases[i].exp_errno);
				continue;
			}
			if (TEST_ERRNO == Test_cases[i].exp_errno) {
				tst_resm(TPASS,
					 "setgroups(%d) fails, %s, errno=%d",
					 gidsetsize, test_desc, TEST_ERRNO);
			} else {
				tst_resm(TFAIL, "setgroups(%d) fails, %s, "
					 "errno=%d, expected errno=%d",
					 gidsetsize, test_desc, TEST_ERRNO,
					 Test_cases[i].exp_errno);
			}
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
}
int setup1(void)
{
	ltpuser = getpwnam(nobody_uid);
	if (seteuid(ltpuser->pw_uid) == -1) {
		tst_resm(TINFO, "setreuid failed to "
			 "to set the effective uid to %d", ltpuser->pw_uid);
		perror("setreuid");
	}
	if ((user_info = getpwnam(TESTUSER)) == NULL) {
		tst_brkm(TFAIL, cleanup, "getpwnam(2) of %s Failed", TESTUSER);
	}
	if (!GID_SIZE_CHECK(user_info->pw_gid)) {
		tst_brkm(TBROK,
			 cleanup,
			 "gid returned from getpwnam is too large for testing setgroups16");
	}
	groups_list[0] = user_info->pw_gid;
	return 0;
}
void cleanup(void)
{
}

