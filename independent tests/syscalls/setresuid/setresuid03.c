#include "incl.h"

uid_t root_uid, main_uid, other_uid, neg_one = -1;

struct test_data_t {
	uid_t *real_uid;
	uid_t *eff_uid;
	uid_t *sav_uid;
	int exp_errno;
	uid_t *exp_real_uid;
	uid_t *exp_eff_uid;
	uid_t *exp_sav_uid;
	char *test_msg;
} test_data[] = {
	{&other_uid, &neg_one, &neg_one, EPERM, &root_uid, &main_uid,
		&main_uid, "setresuid(other, -1, -1)"},
	{&neg_one, &neg_one, &other_uid, EPERM, &root_uid, &main_uid,
		&main_uid, "setresuid(-1, -1, other)"},
	{&neg_one, &other_uid, &neg_one, EPERM, &root_uid, &main_uid,
		&main_uid, "setresuid(-1, other, -1)"}
};

void setup(void)
{
	uid_t test_users[2];
	root_uid = getuid();
	tst_get_uids(test_users, 0, 2);
	main_uid = test_users[0];
	other_uid = test_users[1];
	UID16_CHECK(root_uid, setresuid);
	UID16_CHECK(main_uid, setresuid);
	UID16_CHECK(other_uid, setresuid);
	setresuid(root_uid, main_uid, main_uid);
}

void run(unsigned int n)
{
	const struct test_data_t *tc = test_data + n;
	TST_EXP_FAIL(SETRESUID(*tc->real_uid, *tc->eff_uid, *tc->sav_uid),
		tc->exp_errno, "%s", tc->test_msg);
	if (!TST_PASS)
		return;
	tst_check_resuid(tc->test_msg, *tc->exp_real_uid, *tc->exp_eff_uid,
		*tc->exp_sav_uid);
}

