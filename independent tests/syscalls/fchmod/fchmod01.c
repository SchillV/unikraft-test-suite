#include "incl.h"

int fd;

int modes[] = { 0, 07, 070, 0700, 0777, 02777, 04777, 06777 };

int  verify_fchmod(
{
	struct stat stat_buf;
	int ind;
	mode_t file_mode, mode;
	for (ind = 0; ind < 8; ind++) {
		mode = (mode_t)modes[ind];
fchmod(fd, mode);
		if (TST_RET == -1)
			tst_res(TFAIL | TTERRNO, "fchmod() failed unexpectly");
		fstat(fd, &stat_buf);
		file_mode = stat_buf.st_mode;
		if ((file_mode & ~S_IFREG) != mode) {
			tst_res(TFAIL,
				"%s: Incorrect modes 0%03o, Expected 0%03o",
				TESTFILE, file_mode & ~S_IFREG, mode);
		} else {
			tst_res(TPASS,
				"Functionality of fchmod(%d, %#o) successful",
				fd, mode);
		}
	}
}

void setup(void)
{
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
