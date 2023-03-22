#include "incl.h"

struct passwd *nobody;

void run(void)
{
	TST_EXP_PASS(SETGID(nobody->pw_gid));
	if (getgid() != nobody->pw_gid)
		tst_res(TFAIL, "setgid failed to set gid to nobody gid");
	else
		tst_res(TPASS, "functionality of getgid() is correct");
}

void setup(void)
{
	nobody = getpwnam("nobody");
	GID16_CHECK(nobody->pw_gid, setgid);
}

void main(){
	setup();
	cleanup();
}
