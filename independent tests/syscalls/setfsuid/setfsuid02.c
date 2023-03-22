#include "incl.h"

void run(void)
{
	uid_t invalid_uid, current_uid;
	current_uid = geteuid();
	invalid_uid = (UID_T)-1;
	UID16_CHECK(invalid_uid, setfsuid);
	TST_EXP_VAL_SILENT(SETFSUID(invalid_uid), (long)current_uid);
	TST_EXP_VAL(SETFSUID(invalid_uid), (long)current_uid,
		    "setfsuid(invalid_fsuid) test for expected failure:");
}

void main(){
	setup();
	cleanup();
}
