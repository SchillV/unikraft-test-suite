#include "incl.h"

uid_t nobody_uid;

void setup(void)
{
	struct passwd *nobody;
	nobody = getpwnam("nobody");
	nobody_uid = nobody->pw_uid;
}

void run(void)
{
	uid_t uid;
	uid = geteuid();
	UID16_CHECK(uid, setfsuid);
	seteuid(0);
	TST_EXP_VAL(SETFSUID(nobody_uid), uid, "setfsuid(%d)", nobody_uid);
	TST_EXP_VAL(SETFSUID(-1), nobody_uid);
}

void main(){
	setup();
	cleanup();
}
