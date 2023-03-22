#include "incl.h"

struct passwd *ltpuser;

void setup(void)
{
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
}

int  setegid_verify(
{
	TST_EXP_FAIL(setegid(ltpuser->pw_gid),
				EPERM,
				"setegid(%d)",
				ltpuser->pw_gid);
}

void main(){
	setup();
	cleanup();
}
