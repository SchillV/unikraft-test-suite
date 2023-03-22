#include "incl.h"
#define ROOT_USER	0

int  verify_setuid(
{
	TST_EXP_FAIL(SETUID(ROOT_USER), EPERM);
}

void setup(void)
{
	struct passwd *pw;
	uid_t uid;
	pw = getpwnam("nobody");
	uid = pw->pw_uid;
	setuid(uid);
	umask(0);
}

void main(){
	setup();
	cleanup();
}
