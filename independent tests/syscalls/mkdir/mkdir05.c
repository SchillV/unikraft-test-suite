#include "incl.h"
#define PERMS		0777
#define TESTDIR		"testdir"

int  verify_mkdir(
{
	struct stat buf;
mkdir(TESTDIR, PERMS);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "mkdir() Failed");
		return;
	}
	stat(TESTDIR, &buf);
	if (buf.st_uid != geteuid()) {
		tst_res(TFAIL, "mkdir() FAILED to set owner ID "
			"as process's effective ID");
		return;
	}
	if (buf.st_gid != getegid()) {
		tst_res(TFAIL, "mkdir() failed to set group ID "
			"as the process's group ID");
		return;
	}
	tst_res(TPASS, "mkdir() functionality is correct");
	rmdir(TESTDIR);
}
void setup(void)
{
	struct passwd *pw;
	pw = getpwnam("nobody");
	setuid(pw->pw_uid);
}

void main(){
	setup();
	cleanup();
}
