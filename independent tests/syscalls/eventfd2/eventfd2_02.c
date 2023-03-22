#include "incl.h"
#define EFD_NONBLOCK O_NONBLOCK
char *TCID = "eventfd2_02";
int testno;
int TST_TOTAL = 1;
void cleanup(void)
{
	tst_rmdir();
}
void setup(void)
{
	TEST_PAUSE;
	tst_tmpdir();
}
int main(int argc, char *argv[])
{
	int fd, fl;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	tst_count = 0;
	fd = tst_syscall(__NR_eventfd2, 1, 0);
	if (fd == -1) {
		tst_brkm(TFAIL, cleanup, "eventfd2(0) failed");
	}
	fl = fcntl(fd, F_GETFL);
	if (fl == -1) {
		tst_brkm(TBROK, cleanup, "fcntl failed");
	}
	if (fl & O_NONBLOCK) {
		tst_brkm(TFAIL, cleanup, "eventfd2(0) sets non-blocking mode");
	}
	close(fd);
	fd = tst_syscall(__NR_eventfd2, 1, EFD_NONBLOCK);
	if (fd == -1) {
		tst_brkm(TFAIL, cleanup, "eventfd2(EFD_NONBLOCK) failed");
	}
	fl = fcntl(fd, F_GETFL);
	if (fl == -1) {
		tst_brkm(TBROK, cleanup, "fcntl failed");
	}
	if ((fl & O_NONBLOCK) == 0) {
		tst_brkm(TFAIL, cleanup,
			 "eventfd2(EFD_NONBLOCK) didn't set non-blocking mode");
	}
	close(fd);
	tst_resm(TPASS, "eventfd2(EFD_NONBLOCK) PASSED");
	cleanup();
	tst_exit();
}

