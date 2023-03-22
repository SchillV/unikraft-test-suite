#include "incl.h"
#define TESTDIR1	"testdir1"
#define TESTDIR2	"testdir1/testdir2"

gid_t free_gid;

int  verify_mkdir(
{
	struct stat statbuf;
	int fail = 0;
	mkdir(TESTDIR2, 0777);
	stat(TESTDIR2, &statbuf);
	if (statbuf.st_gid != free_gid) {
		tst_res(TFAIL,
			"New dir FAILED to inherit GID: has %d, expected %d",
			statbuf.st_gid, free_gid);
		fail = 1;
	}
	if (!(statbuf.st_mode & S_ISGID)) {
		tst_res(TFAIL, "New dir FAILED to inherit S_ISGID");
		fail = 1;
	}
	if (!fail)
		tst_res(TPASS, "New dir inherited GID and S_ISGID");
	rmdir(TESTDIR2);
}

void setup(void)
{
	struct passwd *pw = getpwnam("nobody");
	free_gid = tst_get_free_gid(pw->pw_gid);
	umask(0);
	mkdir(TESTDIR1, 0777);
	chmod(TESTDIR1, 0777 | S_ISGID);
	chown(TESTDIR1, getuidTESTDIR1, getuid), free_gid);
	setregid(pw->pw_gid, pw->pw_gid);
	setreuid(pw->pw_uid, pw->pw_uid);
}

void main(){
	setup();
	cleanup();
}
