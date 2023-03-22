#include "incl.h"
struct test_case_t {
	gid_t *rgid;
	gid_t *egid;
	gid_t *sgid;
	gid_t *exp_rgid;
	gid_t *exp_egid;
	gid_t *exp_sgid;
	char *desc;
};

gid_t nobody_gid, other_gid, neg = -1;

struct test_case_t test_cases[] = {
	{&neg, &neg, &other_gid, &nobody_gid, &nobody_gid, &nobody_gid,
		"setresgid(-1, -1, other)"},
	{&neg, &other_gid, &neg, &nobody_gid, &nobody_gid, &nobody_gid,
		"setresgid(-1, other, -1)"},
	{&other_gid, &neg, &neg, &nobody_gid, &nobody_gid, &nobody_gid,
		"setresgid(other, -1, -1)"},
	{&other_gid, &other_gid, &other_gid, &nobody_gid, &nobody_gid,
		&nobody_gid, "setresgid(other, other, other)"},
};

void setup(void)
{
	gid_t test_groups[2];
	struct passwd *pw = getpwnam("nobody");
	nobody_gid = test_groups[0] = pw->pw_gid;
	tst_get_gids(test_groups, 1, 2);
	other_gid = test_groups[1];
	GID16_CHECK(nobody_gid, setresgid);
	GID16_CHECK(other_gid, setresgid);
	setresgid(nobody_gid, nobody_gid, nobody_gid);
	setuid(pw->pw_uid);
}

void run(unsigned int n)
{
	const struct test_case_t *tc = test_cases + n;
	TST_EXP_FAIL(SETRESGID(*tc->rgid, *tc->egid, *tc->sgid), EPERM, "%s",
		tc->desc);
	if (!TST_PASS)
		return;
	tst_check_resgid(tc->desc, *tc->exp_rgid, *tc->exp_egid,
		*tc->exp_sgid);
}

void main(){
	setup();
	cleanup();
}
