#include "incl.h"

char nobody_uid[] = "nobody";

struct rlimit rlim;

struct tcase {
	int resource;
	struct rlimit *rlim;
	int exp_errno;
} tcases[] = {
	{-1, &rlim, EINVAL},
	{RLIMIT_NOFILE, &rlim, EPERM}
};

int  verify_setrlimit(unsigned int n)
{
	struct tcase *tc = &tcases[n];
setrlimit(tc->resource, tc->rlim);
	if (TST_RET != -1) {
		tst_res(TFAIL, "call succeeded unexpectedly");
		return;
	}
	if (TST_ERR != tc->exp_errno) {
		tst_res(TFAIL | TTERRNO,
			"setrlimit() should fail with %s got",
			tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "setrlimit() failed as expected");
}

void setup(void)
{
	struct passwd *ltpuser = getpwnam(nobody_uid);
	setuid(ltpuser->pw_uid);
	getrlimit(RLIMIT_NOFILE, &rlim);
	rlim.rlim_max++;
}

void main(){
	setup();
	cleanup();
}
