#include "incl.h"

gid_t root_gid, nobody_gid, other_gid, neg_one = -1;

struct passwd *ltpuser;

struct tcase {
	gid_t *real_gid;
	gid_t *eff_gid;
	int exp_errno;
	gid_t *exp_real_usr;
	gid_t *exp_eff_usr;
	char *test_msg;
} tcases[] = {
	{
	&neg_one, &root_gid, EPERM, &nobody_gid, &nobody_gid,
		    "After setregid(-1, root),"}, {
	&neg_one, &other_gid, EPERM, &nobody_gid, &nobody_gid,
		    "After setregid(-1, other)"}, {
	&root_gid, &neg_one, EPERM, &nobody_gid, &nobody_gid,
		    "After setregid(root,-1),"}, {
	&other_gid, &neg_one, EPERM, &nobody_gid, &nobody_gid,
		    "After setregid(other, -1),"}, {
	&root_gid, &other_gid, EPERM, &nobody_gid, &nobody_gid,
		    "After setregid(root, other)"}, {
	&other_gid, &root_gid, EPERM, &nobody_gid, &nobody_gid,
		    "After setregid(other, root),"}
};
int  gid_verify(gid_t rg, gid_t eg, char *when)
{
	if ((getgid() != rg) || (getegid() != eg)) {
		tst_res(TFAIL, "ERROR: %s real gid = %d; effective gid = %d",
			 when, getgid(), getegid());
		tst_res(TINFO, "Expected: real gid = %d; effective gid = %d",
			 rg, eg);
		return;
	}
	tst_res(TPASS, "real or effective gid wasn't modified as expected");
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
SETREGID(*tc->real_gid, *tc->eff_gid);
	if (TST_RET == -1) {
		if (tc->exp_errno == TST_ERR) {
			tst_res(TPASS | TTERRNO,
				"setregid(%d, %d) failed as expected",
				*tc->real_gid, *tc->eff_gid);
		} else {
			tst_res(TFAIL | TTERRNO,
				"setregid(%d, %d) failed unexpectedly, expected %s",
				*tc->real_gid, *tc->eff_gid,
				tst_strerrno(tc->exp_errno));
		}
	} else {
		tst_res(TFAIL,
			"setregid(%d, %d) did not fail (ret: %ld) as expected (ret: -1).",
			*tc->real_gid, *tc->eff_gid, TST_RET);
	}
int 	gid_verify(*tc->exp_real_usr, *tc->exp_eff_usr, tc->test_msg);
}

void setup(void)
{
	gid_t test_groups[3];
	ltpuser = getpwnam("nobody");
	nobody_gid = test_groups[0] = ltpuser->pw_gid;
	root_gid = test_groups[1] = getgid();
	tst_get_gids(test_groups, 2, 3);
	other_gid = test_groups[2];
	GID16_CHECK(root_gid, setregid);
	GID16_CHECK(nobody_gid, setregid);
	GID16_CHECK(other_gid, setregid);
	setgid(ltpuser->pw_gid);
	setuid(ltpuser->pw_uid);
}

void main(){
	setup();
	cleanup();
}
