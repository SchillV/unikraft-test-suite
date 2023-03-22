#include "incl.h"
#define TFD_CLOEXEC O_CLOEXEC
char *TCID = "timerfd02";
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
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			fd = tst_syscall(__NR_timerfd_create,
				CLOCK_REALTIME, 0);
			if (fd == -1) {
				tst_brkm(TFAIL, cleanup,
					 "timerfd_create(0) failed");
			}
			coe = fcntl(fd, F_GETFD);
			if (coe == -1) {
				tst_brkm(TBROK, cleanup, "fcntl failed");
			}
			if (coe & FD_CLOEXEC) {
				tst_brkm(TFAIL,
					 cleanup,
					 "timerfd_create(0) set close-on-exec flag");
			}
			close(fd);
			fd = tst_syscall(__NR_timerfd_create, CLOCK_REALTIME,
				     TFD_CLOEXEC);
			if (fd == -1) {
				tst_brkm(TFAIL,
					 cleanup,
					 "timerfd_create(TFD_CLOEXEC) failed");
			}
			coe = fcntl(fd, F_GETFD);
			if (coe == -1) {
				tst_brkm(TBROK, cleanup, "fcntl failed");
			}
			if ((coe & FD_CLOEXEC) == 0) {
				tst_brkm(TFAIL,
					 cleanup,
					 "timerfd_create(TFD_CLOEXEC) set close-on-exec flag");
			}
			close(fd);
			tst_resm(TPASS, "timerfd_create(TFD_CLOEXEC) Passed");
			cleanup();
		}
	}
	tst_exit();
}

