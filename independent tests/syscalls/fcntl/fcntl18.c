#include "incl.h"
#define INVAL_FLAG	-1
#define INVAL_MIN	(-2147483647L-1L)
int fd;
char string[40] = "";
char *TCID = "fcntl18";
int TST_TOTAL = 1;
struct passwd *pass;
void setup(void);
void cleanup(void);
int fail;
int main(int ac, char **av)
{
	int retval;
	struct flock fl;
	int pid, status;
	tst_parse_opts(ac, av, NULL, NULL);
#ifndef UCLINUX
	tst_resm(TINFO, "Enter block 1");
	fail = 0;
		tst_resm(TFAIL, "file opening error");
		fail = 1;
	}
	retval = fcntl(fd, F_GETLK, (struct flock *)INVAL_FLAG);
	if (errno == EFAULT) {
		tst_resm(TPASS, "Test F_GETLK: for errno EFAULT PASSED");
	} else {
		tst_resm(TFAIL, "Test F_GETLK: for errno EFAULT FAILED");
		fail = 1;
	}
	if (fail) {
		tst_resm(TINFO, "Block 1 FAILED");
	} else {
		tst_resm(TINFO, "Block 1 PASSED");
	}
	tst_resm(TINFO, "Exit block 1");
#else
	tst_resm(TINFO, "Skip block 1 on uClinux");
#endif
#ifndef UCLINUX
	tst_resm(TINFO, "Enter block 2");
	fail = 0;
	retval = fcntl(fd, F_GETLK, (struct flock *)INVAL_FLAG);
	if (errno == EFAULT) {
		tst_resm(TPASS, "Test F_GETLK: for errno EFAULT PASSED");
	} else {
		tst_resm(TFAIL, "Test F_GETLK: for errno EFAULT FAILED");
		fail = 1;
	}
	if (fail) {
		tst_resm(TINFO, "Block 2 FAILED");
	} else {
		tst_resm(TINFO, "Block 2 PASSED");
	}
	tst_resm(TINFO, "Exit block 2");
#else
	tst_resm(TINFO, "Skip block 2 on uClinux");
#endif
	tst_resm(TINFO, "Enter block 3");
	fail = 0;
		fail = 0;
		pass = getpwnam("nobody");
		retval = setreuid(-1, pass->pw_uid);
		if (retval < 0) {
			tst_resm(TFAIL, "setreuid to user nobody failed, "
				 "errno: %d", errno);
			fail = 1;
		}
		retval = fcntl(fd, INVAL_FLAG, &fl);
		if (errno == EINVAL) {
			tst_resm(TPASS, "Test for errno EINVAL PASSED");
		} else {
			tst_resm(TFAIL, "Test for errno EINVAL FAILED, "
				 "got: %d", errno);
			fail = 1;
		}
		exit(fail);
		waitpid(pid, &status, 0);
		if (WEXITSTATUS(status) != 0) {
			tst_resm(TFAIL, "child returned bad exit status");
			fail = 1;
		}
		if (fail) {
			tst_resm(TINFO, "Block 3 FAILED");
		} else {
			tst_resm(TINFO, "Block 3 PASSED");
		}
	}
	tst_resm(TINFO, "Exit block 3");
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	tst_require_root();
	umask(0);
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(string, "./fcntl18.%d.1", getpid());
	unlink(string);
}
void cleanup(void)
{
	 * print timing status if that option was specified.
	 * print errno log if that option was specified
	 */
	close(fd);
	tst_rmdir();
	unlink("temp.dat");
}

