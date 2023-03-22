#include "incl.h"
char *TCID = "fcntl29";
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);

int test_fd;
int main(int ac, char **av)
{
	int lc, dup_fd;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
fcntl(test_fd, F_DUPFD_CLOEXEC, 0);
		if (TEST_RETURN < 0) {
			tst_brkm(TFAIL | TTERRNO, cleanup, "fcntl "
				 "test F_DUPFD_CLOEXEC failed");
		}
		dup_fd = TEST_RETURN;
fcntl(dup_fd, F_GETFD);
		if (TEST_RETURN < 0) {
			close(cleanup, dup_fd);
			tst_brkm(TFAIL | TTERRNO, cleanup, "fcntl "
				 "test F_GETFD failed");
		}
		if (TEST_RETURN & FD_CLOEXEC) {
			tst_resm(TPASS, "fcntl test "
				 "F_DUPFD_CLOEXEC success");
		} else {
			tst_resm(TFAIL, "fcntl test "
				 "F_DUPFD_CLOEXEC fail");
		}
		close(cleanup, dup_fd);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	TEST_PAUSE;
	test_fd = creat(cleanup, "testfile", 0644);
}

void cleanup(void)
{
	if (test_fd > 0)
		close(NULL, test_fd);
	tst_rmdir();
}

