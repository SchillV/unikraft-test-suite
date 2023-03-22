#include "incl.h"

uid_t root_uid, nobody_uid, other_uid, neg_one = -1;

struct test_data_t {
	uid_t *real_uid;
	uid_t *eff_uid;
	uid_t *exp_real_uid;
	uid_t *exp_eff_uid;
	uid_t *exp_sav_uid;
	const char *test_msg;
} test_data[] = {
	{&neg_one, &neg_one, &root_uid, &root_uid, &root_uid,
		"setreuid(-1, -1)"},
	{&nobody_uid, &neg_one, &nobody_uid, &root_uid, &root_uid,
		"setreuid(nobody, -1)"},
	{&root_uid, &neg_one, &root_uid, &root_uid, &root_uid,
		"setreuid(root, -1)"},
	{&neg_one, &nobody_uid, &root_uid, &nobody_uid, &nobody_uid,
		"setreuid(-1, nobody)"},
	{&neg_one, &root_uid, &root_uid, &root_uid, &nobody_uid,
		"setreuid(-1, root)"},
	{&other_uid, &neg_one, &other_uid, &root_uid, &root_uid,
		"setreuid(other, -1)"},
	{&root_uid, &neg_one, &root_uid, &root_uid, &root_uid,
		"setreuid(root, -1)"},
};

void setup(void)
{
	uid_t test_users[2];
	struct passwd *pw;
	root_uid = getuid();
	pw = getpwnam("nobody");
	nobody_uid = test_users[0] = pw->pw_uid;
	tst_get_uids(test_users, 1, 2);
	other_uid = test_users[1];
	UID16_CHECK(root_uid, setreuid);
	UID16_CHECK(nobody_uid, setreuid);
	UID16_CHECK(other_uid, setreuid);
	setuid(root_uid);
}

void run(unsigned int n)
{
	const struct test_data_t *tc = test_data + n;
	TST_EXP_PASS_SILENT(SETREUID(*tc->real_uid, *tc->eff_uid), "%s",
		tc->test_msg);
	if (!TST_PASS)
		return;
	if (tst_check_resuid(tc->test_msg, *tc->exp_real_uid, *tc->exp_eff_uid,
		*tc->exp_sav_uid))
		tst_res(TPASS, "%s works as expected", tc->test_msg);
}

void main(){
	setup();
	cleanup();
}
