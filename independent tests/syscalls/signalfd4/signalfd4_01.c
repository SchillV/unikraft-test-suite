#include "incl.h"
#define SFD_CLOEXEC O_CLOEXEC
char *TCID = "signalfd4_01";
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
	sigset_t ss;
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
			coe = fcntl(fd, F_GETFD);
			if (coe == -1) {
				tst_brkm(TBROK, cleanup, "fcntl failed");
			}
			if (coe & FD_CLOEXEC) {
				tst_brkm(TFAIL,
					 cleanup,
					 "signalfd4(0) set close-on-exec flag");
			}
			close(fd);
			fd = tst_syscall(__NR_signalfd4, -1, &ss, SIGSETSIZE,
				     SFD_CLOEXEC);
			if (fd == -1) {
				tst_brkm(TFAIL,
					 cleanup,
					 "signalfd4(SFD_CLOEXEC) failed");
			}
			coe = fcntl(fd, F_GETFD);
			if (coe == -1) {
				tst_brkm(TBROK, cleanup, "fcntl failed");
			}
			if ((coe & FD_CLOEXEC) == 0) {
				tst_brkm(TFAIL,
					 cleanup,
					 "signalfd4(SFD_CLOEXEC) does not set close-on-exec flag");
			}
			close(fd);
			tst_resm(TPASS, "signalfd4(SFD_CLOEXEC) Passed");
			cleanup();
		}
	}
	tst_exit();
}

