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
	uid_t ruid, euid, suid;
	getresuid(&ruid, &euid, &suid);
	seteuid(nobody_uid);
	UID16_CHECK(ruid, setfsuid);
	TST_EXP_VAL_SILENT(SETFSUID(ruid), nobody_uid);
	TST_EXP_VAL(SETFSUID(-1), ruid, "setfsuid(fsuid) by non-root user:");
}

void main(){
	setup();
	cleanup();
}
