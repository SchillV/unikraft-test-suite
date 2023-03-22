#include "incl.h"

int  verify_setuid(
{
	uid_t uid;
	uid = getuid();
	UID16_CHECK(uid, setuid);
	TST_EXP_PASS(SETUID(uid), "setuid(%d)", uid);
}

void main(){
	setup();
	cleanup();
}
