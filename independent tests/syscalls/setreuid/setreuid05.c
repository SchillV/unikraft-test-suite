#include "incl.h"

uid_t root_uid, nobody_uid, main_uid, other_uid, neg_one = -1;
struct test_data_t {
	uid_t *real_uid;
	uid_t *eff_uid;
	int exp_ret;
	uid_t *exp_real_uid;
	uid_t *exp_eff_uid;
	uid_t *exp_sav_uid;
	const char *test_msg;
} test_data[] = {
	{&nobody_uid, &root_uid, 0, &nobody_uid, &root_uid, &root_uid,
		"setreuid(nobody, root)"},
	{&neg_one, &nobody_uid, 0, &nobody_uid, &nobody_uid, &root_uid,
		"setreuid(-1, nobody)"},
	{&neg_one, &root_uid, 0, &nobody_uid, &root_uid, &root_uid,
		"setreuid(-1, root)"},
	{&main_uid, &neg_one, 0, &main_uid, &root_uid, &root_uid,
		"setreuid(main, -1)"},
	{&neg_one, &other_uid, 0, &main_uid, &other_uid, &other_uid,
		"setreuid(-1, other)"},
	{&neg_one, &root_uid, -1, &main_uid, &other_uid, &other_uid,
		"setreuid(-1, root)"},
	{&neg_one, &nobody_uid, -1, &main_uid, &other_uid, &other_uid,
		"setreuid(-1, nobody)"},
	{&neg_one, &main_uid, 0, &main_uid, &main_uid, &other_uid,
		"setreuid(-1, main)"},
	{&neg_one, &other_uid, 0, &main_uid, &other_uid, &other_uid,
		"setreuid(-1, other)"},
	{&other_uid, &main_uid, 0, &other_uid, &main_uid, &main_uid,
		"setreuid(other, main)"},
	{&neg_one, &other_uid, 0, &other_uid, &other_uid, &main_uid,
		"setreuid(-1, other)"},
	{&neg_one, &main_uid, 0, &other_uid, &main_uid, &main_uid,
		"setreuid(-1, main)"},
	{&main_uid, &neg_one, 0, &main_uid, &main_uid, &main_uid,
		"setreuid(main, -1)"},
	{&neg_one, &other_uid, -1, &main_uid, &main_uid, &main_uid,
		"setreuid(-1, other)"},
};

void setup(void)
{
	uid_t test_users[3];
	struct passwd *pw;
	root_uid = getuid();
	pw = getpwnam("nobody");
	nobody_uid = test_users[0] = pw->pw_uid;
	tst_get_uids(test_users, 1, 3);
	main_uid = test_users[1];
	other_uid = test_users[2];
	UID16_CHECK(root_uid, setreuid);
	UID16_CHECK(nobody_uid, setreuid);
	UID16_CHECK(main_uid, setreuid);
	UID16_CHECK(other_uid, setreuid);
	setuid(root_uid);
}

void run_child(const struct test_data_t *tc)
{
	if (tc->exp_ret) {
		TST_EXP_FAIL(SETREUID(*tc->real_uid, *tc->eff_uid), EPERM,
			"%s", tc->test_msg);
	} else {
		TST_EXP_PASS(SETREUID(*tc->real_uid, *tc->eff_uid), "%s",
			tc->test_msg);
	}
	if (!TST_PASS)
		return;
	tst_check_resuid(tc->test_msg, *tc->exp_real_uid, *tc->exp_eff_uid,
		*tc->exp_sav_uid);
}

void run(void)
{
	if (!fork()) {
		unsigned int i;
		for (i = 0; i < ARRAY_SIZE(test_data); i++)
			run_child(test_data + i);
		exit(0);
	}
	tst_reap_children();
}

