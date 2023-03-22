#include "incl.h"

gid_t nobody_gid;

void setup(void)
{
	struct passwd *nobody;
	nobody = getpwnam("nobody");
	nobody_gid = nobody->pw_gid;
}

void run(void)
{
	gid_t gid;
	gid = getegid();
	GID16_CHECK(gid, setfsgid);
	seteuid(0);
	TST_EXP_VAL(SETFSGID(nobody_gid), gid);
	TST_EXP_VAL(SETFSGID(-1), nobody_gid);
	TST_EXP_VAL_SILENT(SETFSGID(gid), nobody_gid);
}

void main(){
	setup();
	cleanup();
}
