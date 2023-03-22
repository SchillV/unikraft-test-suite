#include "incl.h"

struct passwd *ltpuser;

uid_t other_uid;

void setup(void)
{
	tst_get_uids(&other_uid, 0, 1);
	UID16_CHECK(other_uid, setreuid);
	ltpuser = getpwnam("nobody");
	setuid(ltpuser->pw_uid);
}

void run(void)
{
	TST_EXP_FAIL(SETREUID(-1, other_uid), EPERM,
				"setreuid(%d, %d)", -1, other_uid);
	TST_EXP_FAIL(SETREUID(other_uid, -1), EPERM,
				"setreuid(%d, %d)", other_uid, -1);
	TST_EXP_FAIL(SETREUID(other_uid, other_uid), EPERM,
				"setreuid(%d, %d)", other_uid, other_uid);
}

void main(){
	setup();
	cleanup();
}
