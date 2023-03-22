#include "incl.h"
#define SFD_NONBLOCK O_NONBLOCK
char *TCID = "signalfd4_02";
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
	sigset_t ss;
	int fd, fl;
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); ++lc) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; ++testno) {
			sigemptyset(&ss);
			sigaddset(&ss, SIGUSR1);
			fd = tst_syscall(__NR_signalfd4, -1, &ss,
				SIGSETSIZE, 0);
			if (fd == -1) {
				tst_brkm(TFAIL, cleanup,
					 "signalfd4(0) failed");
			}
			fl = fcntl(fd, F_GETFL);
			if (fl == -1) {
				tst_brkm(TBROK, cleanup, "fcntl failed");
			}
			if (fl & O_NONBLOCK) {
				tst_brkm(TFAIL,
					 cleanup,
					 "signalfd4(0) set non-blocking mode");
			}
			close(fd);
			fd = tst_syscall(__NR_signalfd4, -1, &ss, SIGSETSIZE,
				     SFD_NONBLOCK);
			if (fd == -1) {
				tst_brkm(TFAIL,
					 cleanup,
					 "signalfd4(SFD_NONBLOCK) failed");
			}
			fl = fcntl(fd, F_GETFL);
			if (fl == -1) {
				tst_brkm(TBROK, cleanup, "fcntl failed");
			}
			if ((fl & O_NONBLOCK) == 0) {
				tst_brkm(TFAIL,
					 cleanup,
					 "signalfd4(SFD_NONBLOCK) does not set non-blocking mode");
			}
			close(fd);
			tst_resm(TPASS, "signalfd4(SFD_NONBLOCK) PASSED");
			cleanup();
		}
	}
	tst_exit();
}

