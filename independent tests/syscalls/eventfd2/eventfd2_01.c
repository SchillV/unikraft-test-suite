#include "incl.h"
#define EFD_CLOEXEC O_CLOEXEC
char *TCID = "eventfd2_01";
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
	int fd, coe;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	fd = tst_syscall(__NR_eventfd2, 1, 0);
	if (fd == -1) {
		tst_brkm(TFAIL, cleanup, "eventfd2(0) failed");
	}
	coe = fcntl(fd, F_GETFD);
	if (coe == -1) {
		tst_brkm(TBROK, cleanup, "fcntl failed");
	}
	if (coe & FD_CLOEXEC) {
		tst_brkm(TFAIL, cleanup, "eventfd2(0) set close-on-exec flag");
	}
	close(fd);
	fd = tst_syscall(__NR_eventfd2, 1, EFD_CLOEXEC);
	if (fd == -1) {
		tst_brkm(TFAIL, cleanup, "eventfd2(EFD_CLOEXEC) failed");
	}
	coe = fcntl(fd, F_GETFD);
	if (coe == -1) {
		tst_brkm(TBROK, cleanup, "fcntl failed");
	}
	if ((coe & FD_CLOEXEC) == 0) {
		tst_brkm(TFAIL, cleanup,
			 "eventfd2(EFD_CLOEXEC) does not set close-on-exec flag");
	}
	close(fd);
	tst_resm(TPASS, "eventfd2(EFD_CLOEXEC) Passed");
	cleanup();
	tst_exit();
}

