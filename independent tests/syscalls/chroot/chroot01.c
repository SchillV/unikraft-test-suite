#include "incl.h"

char *path;

int  verify_chroot(
{
	TST_EXP_FAIL(chroot(path), EPERM, "unprivileged chroot()");
}

void setup(void)
{
	struct passwd *ltpuser;
	path = tst_get_tmpdir();
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
}

void cleanup(void)
{
	free(path);
}

void main(){
	setup();
	cleanup();
}
