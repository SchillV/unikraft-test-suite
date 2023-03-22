#include "incl.h"
struct passwd *ltpuser;

void do_test(void)
{
	char *new = TST_VALID_DOMAIN_NAME;
do_setdomainname(new, strlen(new));
	if (TST_RET != -1) {
		tst_res(TFAIL, "unexpected exit code: %ld", TST_RET);
		return;
	}
	if (TST_ERR != EPERM) {
		tst_res(TFAIL | TTERRNO, "unexpected errno: %d, expected: EPERM",
			TST_ERR);
		return;
	}
	tst_res(TPASS | TTERRNO, "expected failure");
}
void setup_setuid(void)
{
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
	setup();
}

void cleanup_setuid(void)
{
	seteuid(0);
	cleanup();
}

