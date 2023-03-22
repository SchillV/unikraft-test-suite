#include "incl.h"

void run(void)
{
	uid_t uid;
	struct passwd *pwent;
GETGID();
	if (TST_RET < 0)
		tst_brk(TBROK, "This should never happen");
	uid = getuid();
	pwent = getpwuid(uid);
	if (pwent == NULL)
		tst_brk(TBROK | TERRNO, "getuid() returned unexpected value %d", uid);
	GID16_CHECK(pwent->pw_gid, getgid);
	if (pwent->pw_gid != TST_RET) {
		tst_res(TFAIL, "getgid() return value "
				"%ld unexpected - expected %d",
				TST_RET, pwent->pw_gid);
		return;
	}
	tst_res(TPASS, "values from getgid() and getpwuid() match");
}

void main(){
	setup();
	cleanup();
}
