#include "incl.h"

int fd;

const char nobody_uid[] = "nobody";

struct passwd *ltpuser;

int  verify_fchmod(
{
	struct stat stat_buf;
	mode_t file_mode;
	TST_EXP_PASS_SILENT(fchmod(fd, PERMS));
	if (fstat(fd, &stat_buf) == -1)
		tst_brk(TFAIL | TERRNO, "fstat failed");
	file_mode = stat_buf.st_mode;
	if ((file_mode & PERMS) != PERMS)
		tst_res(TFAIL, "%s: Incorrect modes 0%3o, "
			"Expected 0777", TESTFILE, file_mode);
	else
		tst_res(TPASS, "Functionality of fchmod(%d, "
			"%#o) successful", fd, PERMS);
}

void setup(void)
{
	ltpuser = getpwnam(nobody_uid);
	seteuid(ltpuser->pw_uid);
	fd = open(TESTFILE, O_RDWR | O_CREAT, FILE_MODE);
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
