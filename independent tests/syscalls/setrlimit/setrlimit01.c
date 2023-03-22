#include "incl.h"
char *TCID = "setrlimit01";
int TST_TOTAL = 1;

void setup(void);

void cleanup(void);

void test1(void);

void test2(void);

void test3(void);

void test4(void);

void sighandler(int);

char filename[40] = "";

struct rlimit save_rlim, rlim, rlim1;

int nofiles, fd, bytes, i, status;

char *buf = "abcdefghijklmnopqrstuvwxyz";

pid_t pid;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		test1();
		test2();
		test3();
		if ((setrlimit(RLIMIT_NPROC, &save_rlim)) == -1) {
			tst_brkm(TBROK, cleanup, "setrlimit failed to reset "
				 "RLIMIT_NPROC, errno = %d", errno);
		}
		test4();
	}
	cleanup();
	tst_exit();
}

void test1(void)
{
	rlim.rlim_cur = 100;
	rlim.rlim_max = 100;
setrlimit(RLIMIT_NOFILE, &rlim);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL, "setrlimit failed to set "
			 "RLIMIT_NOFILE, errno = %d", errno);
		return;
	}
	nofiles = getdtablesize();
	if (nofiles != 100) {
		tst_resm(TFAIL, "setrlimit failed, expected "
			 "100, got %d", nofiles);
		return;
	}
	tst_resm(TPASS, "RLIMIT_NOFILE functionality is correct");
}

void test2(void)
{
	 * Since we would be altering the filesize in the child,
	 * we need to "sync", ie. fflush the parent's write buffers
	 * here.  This is because the child will inherit the parent's
	 * write buffer, and while exiting it would try to fflush it.
	 * Since its filesize is truncated to only 10 bytes, the
	 * fflush attempt would fail, and the child would exit with
	 * an wired value!  So, it is essential to fflush the parent's
	 * write buffer HERE
	 */
	int pipefd[2];
	fflush(stdout);
	pipe(NULL, pipefd);
	 * Spawn a child process, and reduce the filesize to
	 * 10 by calling setrlimit(). We can't do this in the
	 * parent, because the parent needs a bigger filesize as its
	 * output will be saved to the logfile (instead of stdout)
	 * when the testcase (parent) is run from the driver.
	 */
	pid = FORK_OR_VFORK();
	if (pid == -1)
		tst_brkm(TBROK, cleanup, "fork() failed");
	if (pid == 0) {
		rlim.rlim_cur = 10;
		rlim.rlim_max = 10;
		if ((setrlimit(RLIMIT_FSIZE, &rlim)) == -1)
			exit(1);
		fd = creat(filename, 0644);
		if (fd < 0)
			exit(2);
		bytes = write(fd, buf, 26);
		if (bytes != 10) {
			if (write(pipefd[1], &bytes, sizeof(bytes)) < (long)sizeof(bytes)) {
				perror("child: write to pipe failed");
			}
			exit(3);
		}
	}
	waitpid(cleanup, pid, &status, 0);
	switch (WEXITSTATUS(status)) {
	case 0:
		tst_resm(TPASS, "RLIMIT_FSIZE test PASSED");
		break;
	case 1:
		tst_resm(TFAIL, "setrlimit failed to set "
			 "RLIMIT_FSIZE, errno = %d", errno);
		break;
	case 2:
		tst_resm(TFAIL, "creating testfile failed");
		break;
	case 3:
		if (read(pipefd[0], &bytes, sizeof(bytes)) < (long)sizeof(bytes))
			tst_resm(TFAIL, "parent: reading pipe failed");
		close(pipefd[0]);
		tst_resm(TFAIL, "setrlimit failed, expected "
			 "10 got %d", bytes);
		break;
	default:
		tst_resm(TFAIL, "child returned bad exit status");
	}
}

void test3(void)
{
	getrlimit(cleanup, RLIMIT_NPROC, &save_rlim);
	rlim.rlim_cur = 10;
	rlim.rlim_max = 10;
setrlimit(RLIMIT_NPROC, &rlim);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL, "setrlimit failed to set "
			 "RLIMIT_NPROC, errno = %d", errno);
		return;
	}
	if ((getrlimit(RLIMIT_NPROC, &rlim1)) == -1) {
		tst_brkm(TBROK, cleanup, "getrlimit failed to get "
			 "values for RLIMIT_NPROC, errno = %d", errno);
	}
	if ((rlim1.rlim_cur != 10) && (rlim1.rlim_max != 10)) {
		tst_resm(TFAIL, "setrlimit did not set the proc "
			 "limit correctly");
		return;
	}
	for (i = 0; i < 20; i++) {
		pid = FORK_OR_VFORK();
		if (pid == -1) {
			if (errno != EAGAIN) {
				tst_resm(TWARN, "Expected EAGAIN got %d",
					 errno);
			}
		} else if (pid == 0) {
			exit(0);
		}
	}
	waitpid(pid, &status, 0);
	if (WEXITSTATUS(status) != 0)
		tst_resm(TFAIL, "RLIMIT_NPROC functionality is not correct");
	else
		tst_resm(TPASS, "RLIMIT_NPROC functionality is correct");
}

void test4(void)
{
	rlim.rlim_cur = 10;
	rlim.rlim_max = 10;
setrlimit(RLIMIT_CORE, &rlim);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL | TTERRNO, "setrlimit failed to set RLIMIT_CORE");
		return;
	}
	pid = FORK_OR_VFORK();
	if (pid == -1)
		tst_brkm(TBROK, cleanup, "fork() failed");
		char *testbuf = NULL;
		strcpy(testbuf, "abcd");
		exit(0);
	}
	wait(&status);
	if (access("core", F_OK) == 0) {
		tst_resm(TFAIL, "core dump dumped unexpectedly");
		return;
	} else if (errno != ENOENT) {
		tst_resm(TFAIL | TERRNO, "access failed unexpectedly");
		return;
	}
	tst_resm(TPASS, "RLIMIT_CORE functionality is correct");
}

void sighandler(int sig)
{
	if (sig != SIGSEGV && sig != SIGXFSZ && sig != SIGTERM)
		tst_brkm(TBROK, NULL, "caught unexpected signal: %d", sig);
	_exit(0);
}

void setup(void)
{
	tst_require_root();
	umask(0);
	tst_sig(FORK, sighandler, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(filename, "setrlimit1.%d", getpid());
}

void cleanup(void)
{
	unlink(filename);
	tst_rmdir();
}

