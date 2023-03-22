#include "incl.h"
#define PERMS_DIR	043777

int fd;

int  verify_fchmod(
{
	struct stat stat_buf;
	mode_t dir_mode;
fchmod(fd, PERMS_DIR);
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "fchmod() failed unexpectly");
	fstat(fd, &stat_buf);
	dir_mode = stat_buf.st_mode;
	if ((PERMS_DIR & ~S_ISGID) != dir_mode) {
		tst_res(TFAIL, "%s: Incorrect modes 0%03o, Expected 0%03o",
			TESTDIR, dir_mode & ~S_ISGID, PERMS_DIR);
	} else {
		tst_res(TPASS, "Functionality of fchmod(%d, %#o) successful",
			fd, PERMS_DIR);
	}
}

void setup(void)
{
	struct passwd *ltpuser;
	gid_t free_gid;
	ltpuser = getpwnam("nobody");
	free_gid = tst_get_free_gid(ltpuser->pw_gid);
	mkdir(TESTDIR, DIR_MODE);
	if (setgroups(1, &ltpuser->pw_gid) == -1) {
		tst_brk(TBROK, "Couldn't change supplementary group Id: %s",
			tst_strerrno(TST_ERR));
	}
	chown(TESTDIR, ltpuser->pw_uid, free_gid);
	setegid(ltpuser->pw_gid);
	seteuid(ltpuser->pw_uid);
	fd = open(TESTDIR, O_RDONLY);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	setegid(0);
	seteuid(0);
}

void main(){
	setup();
	cleanup();
}
