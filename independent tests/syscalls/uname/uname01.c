#include "incl.h"

int  verify_uname(
{
	struct utsname un;
	memset(&un, 0, sizeof(un));
	TST_EXP_PASS(uname(&un));
	if (!TST_PASS)
		return;
	if (strcmp(un.sysname, "Linux")) {
		tst_res(TFAIL, "sysname is not Linux");
		return;
	}
	tst_res(TPASS, "sysname set to Linux");
}

void main(){
	setup();
	cleanup();
}
