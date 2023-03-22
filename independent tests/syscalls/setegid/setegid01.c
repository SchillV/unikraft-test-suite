#include "incl.h"

gid_t nobody_gid;

void setup(void)
{
	struct passwd *nobody;
	nobody = getpwnam("nobody");
	nobody_gid = nobody->pw_gid;
}

int  setegid_verify(
{
	gid_t cur_rgid, cur_egid, cur_sgid;
	gid_t orig_rgid, orig_egid, orig_sgid;
	getresgid(&orig_rgid, &orig_egid, &orig_sgid);
	tst_res(TINFO, "getresgid() reports rgid: %d, egid: %d, sgid: %d",
			orig_rgid, orig_egid, orig_sgid);
	tst_res(TINFO, "call setegid(nobody_gid %d)", nobody_gid);
	setegid(nobody_gid);
	getresgid(&cur_rgid, &cur_egid, &cur_sgid);
	tst_res(TINFO, "getresgid() reports rgid: %d, egid: %d, sgid: %d",
			cur_rgid, cur_egid, cur_sgid);
	TST_EXP_EQ_LU(nobody_gid, cur_egid);
	TST_EXP_EQ_LU(orig_rgid, cur_rgid);
	TST_EXP_EQ_LU(orig_sgid, cur_sgid);
	setegid(orig_egid);
	getresgid(&cur_rgid, &cur_egid, &orig_sgid);
	TST_EXP_EQ_LU(orig_egid, cur_egid);
}

void main(){
	setup();
	cleanup();
}
