#include "incl.h"

int fd;

int  verify_fchmod(
{
	struct stat stat_buf;
	mode_t file_mode;
fchmod(fd, PERMS);
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "fchmod() failed unexpectly");
	fstat(fd, &stat_buf);
	file_mode = stat_buf.st_mode;
	if ((file_mode & ~S_IFREG) != PERMS) {
		tst_res(TFAIL, "%s: Incorrect modes 0%03o, Expected 0%03o",
			TESTFILE, file_mode, PERMS);
	} else {
		tst_res(TPASS, "Functionality of fchmod(%d, %#o) Successful",
			fd, PERMS);
	}
}

void setup(void)
{
	struct passwd *ltpuser;
	struct group *ltpgroup;
	ltpuser = getpwnam("nobody");
	ltpgroup = getgrnam_fallback("users", "daemon");
	fd = open(TESTFILE, O_RDWR | O_CREAT, FILE_MODE);
	chown(TESTFILE, ltpuser->pw_uid, ltpgroup->gr_gid);
	setgid(ltpgroup->gr_gid);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
