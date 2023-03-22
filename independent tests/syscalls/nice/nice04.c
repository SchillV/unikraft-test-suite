#include "incl.h"
#define NICEINC -10

int  verify_nice(
{
nice(NICEINC);
	if (TST_RET != -1) {
		tst_res(TFAIL, "nice(%i) succeded unexpectedly (returned %li)",
			NICEINC, TST_RET);
		return;
	}
	if (TST_ERR != EPERM) {
		tst_res(TFAIL | TTERRNO, "nice(%i) should fail with EPERM",
			NICEINC);
		return;
	}
	tst_res(TPASS, "nice(%i) failed with EPERM", NICEINC);
}

void setup(void)
{
	struct passwd *ltpuser;
	ltpuser = getpwnam("nobody");
	setuid(ltpuser->pw_uid);
}

void main(){
	setup();
	cleanup();
}
