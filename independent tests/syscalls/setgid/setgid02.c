#include "incl.h"

void run(void)
{
	struct passwd *rootpwent;
	rootpwent = getpwnam("root");
	GID16_CHECK(rootpwent->pw_gid, setgid);
	TST_EXP_FAIL(SETGID(rootpwent->pw_gid), EPERM);
}

void setup(void)
{
	struct passwd *nobody = getpwnam("nobody");
	setgid(nobody->pw_gid);
	setuid(nobody->pw_uid);
}

void main(){
	setup();
	cleanup();
}
