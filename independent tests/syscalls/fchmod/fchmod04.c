#include "incl.h"

int fd;

const char nobody_uid[] = "nobody";

int  verify_fchmod(
{
	struct stat stat_buf;
	mode_t dir_mode;
	TST_EXP_PASS_SILENT(fchmod(fd, PERMS));
	if (fstat(fd, &stat_buf) == -1)
		tst_brk(TFAIL | TERRNO, "fstat failed");
	dir_mode = stat_buf.st_mode;
	if ((dir_mode & PERMS) == PERMS)
		tst_res(TPASS, "Functionality of fchmod(%d, "
			"%#o) successful", fd, PERMS);
	else
		tst_res(TFAIL, "%s: Incorrect modes 0%03o, "
			"Expected 0%03o",
			TESTDIR, dir_mode, PERMS);
}

void setup(void)
{
	getpwnam(nobody_uid);
	mkdir(TESTDIR, DIR_MODE);
	fd = open(TESTDIR, O_RDONLY);
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
