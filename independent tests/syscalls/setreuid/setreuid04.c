#include "incl.h"

uid_t root_uid, nobody_uid;

void setup(void)
{
	struct passwd *nobody;
	root_uid = getuid();
	nobody = getpwnam("nobody");
	nobody_uid = nobody->pw_uid;
	UID16_CHECK(nobody_uid, setreuid);
	UID16_CHECK(root_uid, setreuid);
}

void run(void)
{
	if (!fork()) {
		TST_EXP_PASS(SETREUID(nobody_uid, nobody_uid));
		TST_EXP_EQ_LI(GETUID(), nobody_uid);
		TST_EXP_EQ_LI(GETEUID(), nobody_uid);
	}
	tst_reap_children();
}

void main(){
	setup();
	cleanup();
}
