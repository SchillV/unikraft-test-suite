#include "incl.h"

uid_t ruid, euid;

void run(void)
{
	ruid = getuid();
	UID16_CHECK(ruid, setreuid);
	euid = geteuid();
	UID16_CHECK(euid, setreuid);
	TST_EXP_PASS(SETREUID(-1, -1));
	TST_EXP_PASS(SETREUID(-1, euid));
	TST_EXP_PASS(SETREUID(ruid, -1));
	TST_EXP_PASS(SETREUID(-1, ruid));
	TST_EXP_PASS(SETREUID(euid, -1));
	TST_EXP_PASS(SETREUID(euid, euid));
	TST_EXP_PASS(SETREUID(ruid, ruid));
}

void main(){
	setup();
	cleanup();
}
