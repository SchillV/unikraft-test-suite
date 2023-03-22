#include "incl.h"

struct passwd *ltpuser;

void run(void)
{
GETGID();
	if (TST_RET != ltpuser->pw_gid)
		tst_res(TFAIL, "getgid failed, returned %ld", TST_RET);
	else
		tst_res(TPASS, "getgid returned as expectedly");
}

void setup(void)
{
	ltpuser = getpwnam("nobody");
	setgid(ltpuser->pw_gid);
}

void main(){
	setup();
	cleanup();
}
