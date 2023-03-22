#include "incl.h"

int fail = -1;

int pass;

gid_t primary_gid, secondary_gid, neg_one = -1;
struct tcase {
	gid_t *real_gid;
	gid_t *eff_gid;
	int *exp_ret;
	gid_t *exp_real_usr;
	gid_t *exp_eff_usr;
	char *test_msg;
} tcases[] = {
	{
	&primary_gid, &secondary_gid, &pass, &primary_gid, &secondary_gid,
		    "After setregid(primary, secondary),"}, {
	&neg_one, &primary_gid, &pass, &primary_gid, &primary_gid,
		    "After setregid(-1, primary)"}, {
	&neg_one, &secondary_gid, &pass, &primary_gid, &secondary_gid,
		    "After setregid(-1, secondary),"}, {
	&secondary_gid, &neg_one, &pass, &secondary_gid, &secondary_gid,
		    "After setregid(secondary, -1),"}, {
	&neg_one, &neg_one, &pass, &secondary_gid, &secondary_gid,
		    "After setregid(-1, -1),"}, {
	&neg_one, &secondary_gid, &pass, &secondary_gid, &secondary_gid,
		    "After setregid(-1, secondary),"}, {
	&secondary_gid, &neg_one, &pass, &secondary_gid, &secondary_gid,
		    "After setregid(secondary, -1),"}, {
	&secondary_gid, &secondary_gid, &pass, &secondary_gid, &secondary_gid,
		    "After setregid(secondary, secondary),"}, {
	&primary_gid, &neg_one, &fail, &secondary_gid, &secondary_gid,
		    "After setregid(primary, -1)"}, {
	&neg_one, &primary_gid, &fail, &secondary_gid, &secondary_gid,
		    "After setregid(-1, primary)"}, {
	&primary_gid, &primary_gid, &fail, &secondary_gid, &secondary_gid,
		    "After setregid(primary, primary)"},};

void setup(void)
{
	struct passwd *nobody;
	gid_t test_groups[2];
	nobody = getpwnam("nobody");
	tst_get_gids(test_groups, 0, 2);
	primary_gid = test_groups[0];
	secondary_gid = test_groups[1];
	GID16_CHECK(primary_gid, setregid);
	GID16_CHECK(secondary_gid, setregid);
	setregid(primary_gid, secondary_gid);
	seteuid(nobody->pw_uid);
}

void test_success(struct tcase *tc)
{
	if (TST_RET != 0)
		tst_res(TFAIL | TTERRNO, "setregid(%d, %d) failed unexpectedly",
			*tc->real_gid, *tc->eff_gid);
	else
		tst_res(TPASS, "setregid(%d, %d) succeeded as expected",
			*tc->real_gid, *tc->eff_gid);
}

void test_failure(struct tcase *tc)
{
	if (TST_RET == 0)
		tst_res(TFAIL, "setregid(%d, %d) succeeded unexpectedly",
			*tc->real_gid, *tc->eff_gid);
	else if (TST_ERR == EPERM)
		tst_res(TPASS, "setregid(%d, %d) failed as expected",
			*tc->real_gid, *tc->eff_gid);
	else
		tst_res(TFAIL | TTERRNO,
			"setregid(%d, %d) did not set errno value as expected",
			*tc->real_gid, *tc->eff_gid);
}

int  gid_verify(gid_t rg, gid_t eg, char *when)
{
	if ((getgid() != rg) || (getegid() != eg)) {
		tst_res(TFAIL, "ERROR: %s real gid = %d; effective gid = %d",
			 when, getgid(), getegid());
		tst_res(TINFO, "Expected: real gid = %d; effective gid = %d",
			 rg, eg);
	} else {
		tst_res(TPASS,
			"real or effective gid was modified as expected");
	}
}

void run(unsigned int i)
{
	struct tcase *tc = &tcases[i];
SETREGID(*tc->real_gid, *tc->eff_gid);
	if (*tc->exp_ret == 0)
		test_success(tc);
	else
		test_failure(tc);
int 	gid_verify(*tc->exp_real_usr, *tc->exp_eff_usr, tc->test_msg);
}
void run_all(void)
{
	unsigned int i;
	if (!fork()) {
		for (i = 0; i < ARRAY_SIZE(tcases); i++)
			run(i);
	}
}

void main(){
	setup();
	cleanup();
}
