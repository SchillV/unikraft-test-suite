#include "incl.h"
#define _GNU_SOURCE 1
#define EXP_RET_VAL	0
};
TCID_DEFINE(setresgid01);

int testno;

struct passwd nobody, root;

uid_t nobody_gid, root_gid, neg = -1;

int test_functionality(uid_t, uid_t, uid_t);

void setup(void);

void cleanup(void);

struct test_case_t tdat[] = {
	{&neg, &neg, &neg, &root, &root, &root,
	 "setresgid(-1, -1, -1)"},
	{&neg, &neg, &nobody.pw_gid, &root, &root, &nobody,
	 "setresgid(-1, -1, nobody)"},
	{&neg, &nobody.pw_gid, &neg, &root, &nobody, &nobody,
	 "setresgid(-1, nobody, -1)"},
	{&nobody.pw_gid, &neg, &neg, &nobody, &nobody, &nobody,
	 "setresgid(nobody, -1, -1)"},
	{&root.pw_gid, &root.pw_gid, &root.pw_gid, &root, &root, &root,
	 "setresgid(root, root, root)"},
};
int TST_TOTAL = sizeof(tdat) / sizeof(tdat[0]);
int main(int argc, char **argv)
{
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
SETRESGID(cleanup, *tdat[testno].rgid, *tdat[testno].egi;
				       *tdat[testno].sgid));
			if (TEST_RETURN == EXP_RET_VAL) {
				if (!test_functionality
				    (tdat[testno].exp_rgid->pw_gid,
				     tdat[testno].exp_egid->pw_gid,
				     tdat[testno].exp_sgid->pw_gid)) {
					tst_resm(TPASS, "Test for %s "
						 "successful",
						 tdat[testno].desc);
				} else {
					tst_resm(TFAIL, "Functionality test "
						 "for %s failed",
						 tdat[testno].desc);
				}
			} else {
				tst_resm(TFAIL, "Test for %s failed; returned"
					 " %ld (expected %d), errno %d (expected"
					 " 0)", tdat[testno].desc,
					 TEST_RETURN, EXP_RET_VAL, TEST_ERRNO);
			}
		}
	}
	cleanup();
	tst_exit();
}

int test_functionality(uid_t exp_rgid, uid_t exp_egid, uid_t exp_sgid)
{
	uid_t cur_rgid, cur_egid, cur_sgid;
	getresgid(cleanup, &cur_rgid, &cur_egid, &cur_sgid);
	if ((cur_rgid == exp_rgid) && (cur_egid == exp_egid)
	    && (cur_sgid == exp_sgid)) {
		return 0;
	}
	return 1;
}
void setup(void)
{
	struct passwd *passwd_p;
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	if ((passwd_p = getpwnam("root")) == NULL) {
		tst_brkm(TBROK, NULL, "getpwnam() failed for root");
	}
	root = *passwd_p;
	GID16_CHECK((root_gid = root.pw_gid), "setresgid", cleanup)
	if ((passwd_p = getpwnam("nobody")) == NULL) {
		tst_brkm(TBROK, NULL, "nobody user id doesn't exist");
	}
	nobody = *passwd_p;
	GID16_CHECK((nobody_gid = nobody.pw_gid), "setresgid", cleanup)
	 * TEST_PAUSE contains the code to fork the test with the -c option.
	 */
	TEST_PAUSE;
}
void cleanup(void)
{
}

