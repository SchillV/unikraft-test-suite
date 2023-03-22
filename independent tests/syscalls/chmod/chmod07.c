#include "incl.h"
#define FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define TESTFILE	"testfile"
void test_chmod(void)
{
	struct stat stat_buf;
	 * Call chmod(2) with specified mode argument
	 * (sticky-bit set) on testfile.
	 */
chmod(TESTFILE, PERMS);
	if (TST_RET == -1) {
		tst_brk(TFAIL | TTERRNO, "chmod(%s, %#o) failed",
				TESTFILE, PERMS);
	}
	if (stat(TESTFILE, &stat_buf) == -1) {
		tst_brk(TFAIL | TTERRNO, "stat failed");
	}
	if ((stat_buf.st_mode & PERMS) == PERMS) {
		tst_res(TPASS, "Functionality of chmod(%s, %#o) successful",
				TESTFILE, PERMS);
	} else {
		tst_res(TFAIL, "%s: Incorrect modes 0%03o; expected 0%03o",
				TESTFILE, stat_buf.st_mode, PERMS);
	}
}
void setup(void)
{
	struct passwd *ltpuser;
	struct group *ltpgroup;
	int fd;
	gid_t group1_gid;
	uid_t user1_uid;
	ltpuser = getpwnam("nobody");
	user1_uid = ltpuser->pw_uid;
	ltpgroup = getgrnam_fallback("users", "daemon");
	group1_gid = ltpgroup->gr_gid;
	fd = open(TESTFILE, O_RDWR | O_CREAT, FILE_MODE);
	close(fd);
	chown(TESTFILE, user1_uid, group1_gid);
	setgid(group1_gid);
}

void main(){
	setup();
	cleanup();
}
