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

gid_t root_gid, main_gid, other_gid, neg = -1;

struct test_case_t test_cases[] = {
	{&neg, &neg, &neg, &root_gid, &main_gid, &main_gid,
	 "setresgid(-1, -1, -1)"},
	{&neg, &neg, &other_gid, &root_gid, &main_gid, &other_gid,
	 "setresgid(-1, -1, other)"},
	{&neg, &other_gid, &neg, &root_gid, &other_gid, &other_gid,
	 "setresgid(-1, other, -1)"},
	{&other_gid, &neg, &neg, &other_gid, &other_gid, &other_gid,
	 "setresgid(other, -1, -1)"},
	{&root_gid, &root_gid, &root_gid, &root_gid, &root_gid, &root_gid,
	 "setresgid(root, root, root)"},
	{&root_gid, &main_gid, &main_gid, &root_gid, &main_gid, &main_gid,
	 "setresgid(root, main, main)"},
};

void setup(void)
{
	gid_t test_groups[3];
	root_gid = test_groups[0] = getgid();
	tst_get_gids(test_groups, 1, 3);
	main_gid = test_groups[1];
	other_gid = test_groups[2];
	GID16_CHECK(root_gid, setresgid);
	GID16_CHECK(main_gid, setresgid);
	GID16_CHECK(other_gid, setresgid);
	setresgid(-1, main_gid, main_gid);
}

void run(unsigned int n)
{
	const struct test_case_t *tc = test_cases + n;
	TST_EXP_PASS_SILENT(SETRESGID(*tc->rgid, *tc->egid, *tc->sgid), "%s",
		tc->desc);
	if (!TST_PASS)
		return;
	if (tst_check_resgid(tc->desc, *tc->exp_rgid, *tc->exp_egid,
		*tc->exp_sgid))
		tst_res(TPASS, "%s works as expected", tc->desc);
}

