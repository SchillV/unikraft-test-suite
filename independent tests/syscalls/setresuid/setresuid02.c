#include "incl.h"
#define _GNU_SOURCE 1

uid_t nobody_uid, other_uid, neg_one = -1;

struct test_data_t {
	uid_t *real_uid;
	uid_t *eff_uid;
	uid_t *sav_uid;
	uid_t *exp_real_uid;
	uid_t *exp_eff_uid;
	uid_t *exp_sav_uid;
	char *test_msg;
} test_data[] = {
	{&neg_one, &neg_one, &other_uid, &nobody_uid, &other_uid, &other_uid,
		"setresuid(-1, -1, other)"},
	{&neg_one, &nobody_uid, &neg_one, &nobody_uid, &nobody_uid, &other_uid,
		"setresuid(-1, nobody -1)"},
	{&other_uid, &neg_one, &neg_one, &other_uid, &nobody_uid, &other_uid,
		"setresuid(other, -1 -1)"},
	{&nobody_uid, &other_uid, &nobody_uid, &nobody_uid, &other_uid,
		&nobody_uid, "setresuid(nobody, other, nobody)"},
};

void setup(void)
{
	uid_t test_users[2];
	struct passwd *pw = getpwnam("nobody");
	nobody_uid = test_users[0] = pw->pw_uid;
	tst_get_uids(test_users, 1, 2);
	other_uid = test_users[1];
	UID16_CHECK(nobody_uid, setresuid);
	UID16_CHECK(other_uid, setresuid);
	setresuid(nobody_uid, other_uid, nobody_uid);
}

void run(unsigned int n)
{
	const struct test_data_t *tc = test_data + n;
	TST_EXP_PASS_SILENT(SETRESUID(*tc->real_uid, *tc->eff_uid,
		*tc->sav_uid), "%s", tc->test_msg);
	if (!TST_PASS)
		return;
	if (tst_check_resuid(tc->test_msg, *tc->exp_real_uid,
		*tc->exp_eff_uid, *tc->exp_sav_uid))
		tst_res(TPASS, "%s works as expected", tc->test_msg);
}

void main(){
	setup();
	cleanup();
}
