#include "incl.h"
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#define MODE_RWX	(mode_t)(S_IRWXU | S_IRWXG | S_IRWXO)
#define DIR_MODE	(mode_t)(S_ISVTX | S_ISGID | S_IFDIR)
#define PERMS		(mode_t)(MODE_RWX | DIR_MODE)
#define TESTDIR		"testdir"

void test_chmod(void)
{
	struct stat stat_buf;
	mode_t dir_mode;
chmod(TESTDIR, PERMS);
	if (TST_RET == -1) {
		tst_res(TFAIL, "chmod(%s, %#o) failed", TESTDIR, PERMS);
		return;
	}
	stat(TESTDIR, &stat_buf);
	dir_mode = stat_buf.st_mode;
	if ((PERMS & ~S_ISGID) != dir_mode) {
		tst_res(TFAIL, "%s: Incorrect modes 0%03o, "
				"Expected 0%03o", TESTDIR, dir_mode,
				PERMS & ~S_ISGID);
	} else {
		tst_res(TPASS, "Functionality of chmod(%s, %#o) successful",
				TESTDIR, PERMS);
	}
}

void setup(void)
{
	struct passwd *nobody_u;
	gid_t free_gid;
	nobody_u = getpwnam("nobody");
	free_gid = tst_get_free_gid(nobody_u->pw_gid);
	 * Create a test directory under temporary directory with specified
	 * mode permissions and change the gid of test directory to nobody's
	 * gid.
	 */
	mkdir(TESTDIR, MODE_RWX);
	if (setgroups(1, &nobody_u->pw_gid) == -1)
		tst_brk(TBROK | TERRNO, "setgroups to nobody's gid failed");
	chown(TESTDIR, nobody_u->pw_uid, free_gid);
	setegid(nobody_u->pw_gid);
	seteuid(nobody_u->pw_uid);
}

void main(){
	setup();
	cleanup();
}
