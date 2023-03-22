#include "incl.h"
#define TFD_NONBLOCK O_NONBLOCK
char *TCID = "timerfd03";
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
			fl = fcntl(fd, F_GETFL);
			if (fl == -1) {
				tst_brkm(TBROK, cleanup, "fcntl failed");
			}
			if (fl & O_NONBLOCK) {
				tst_brkm(TFAIL,
					 cleanup,
					 "timerfd_create(0) set non-blocking mode");
			}
			close(fd);
			fd = tst_syscall(__NR_timerfd_create, CLOCK_REALTIME,
				     TFD_NONBLOCK);
			if (fd == -1) {
				tst_brkm(TFAIL,
					 cleanup,
					 "timerfd_create(TFD_NONBLOCK) failed");
			}
			fl = fcntl(fd, F_GETFL);
			if (fl == -1) {
				tst_brkm(TBROK, cleanup, "fcntl failed");
			}
			if ((fl & O_NONBLOCK) == 0) {
				tst_brkm(TFAIL,
					 cleanup,
					 "timerfd_create(TFD_NONBLOCK) set non-blocking mode");
			}
			close(fd);
			tst_resm(TPASS, "timerfd_create(TFD_NONBLOCK) PASSED");
			cleanup();
		}
	}
	tst_exit();
}

