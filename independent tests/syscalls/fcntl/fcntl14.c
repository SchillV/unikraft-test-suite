#include "incl.h"
#define _GNU_SOURCE 1
#define SKIP 0x0c00
#if SKIP == F_RDLCK || SKIP== F_WRLCK
#error invalid value for SKIP, must be distinct from F_RDLCK and F_WRLCK
#endif
#ifndef S_ENFMT
#define S_ENFMT S_ISGID
#endif
#define NOBLOCK 2
#define WILLBLOCK 3
#define TIME_OUT 60
typedef struct {
	short a_type;
	short a_whence;
	long a_start;
	long a_len;
	short b_whence;
	long b_start;
	long b_len;
	short c_type;
	int c_whence;
	long c_start;
	long c_len;
	short c_flag;
} testcase;

testcase testcases[] = {
	{F_WRLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 0L, 0L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 0L, 0L, WILLBLOCK},
	{F_RDLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 0L, 0L, NOBLOCK},
	{F_RDLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 0L, 0L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 0L, 5L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 0L, 5L, WILLBLOCK},
	{F_RDLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 0L, 5L, NOBLOCK},
	{F_RDLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 0L, 5L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 7L, 0L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 7L, 0L, WILLBLOCK},
	{F_RDLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 7L, 0L, NOBLOCK},
	{F_RDLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 7L, 0L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 0L, 5L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 0L, 5L, WILLBLOCK},
	{F_RDLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 0L, 5L, NOBLOCK},
	{F_RDLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 0L, 5L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 1L, 3L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 1L, 3L, WILLBLOCK},
	{F_RDLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 1L, 3L, NOBLOCK},
	{F_RDLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 1L, 3L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 2L, 5L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 2L, 5L, WILLBLOCK},
	{F_RDLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 2L, 5L, NOBLOCK},
	{F_RDLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 2L, 5L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 6L, 0L, NOBLOCK},
	{F_WRLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 6L, 0L, NOBLOCK},
	{F_RDLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 6L, 0L, NOBLOCK},
	{F_RDLCK, 0, 0L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 6L, 0L, NOBLOCK},
	{F_WRLCK, 0, 2L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 0L, 2L, NOBLOCK},
	{F_WRLCK, 0, 2L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 0L, 2L, NOBLOCK},
	{F_RDLCK, 0, 2L, 5L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 0L, 2L, NOBLOCK},
	{F_RDLCK, 0, 2L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 0L, 2L, NOBLOCK},
	{F_WRLCK, 0, 4L, 0L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 2L, 5L, WILLBLOCK},
	{F_WRLCK, 0, 4L, 0L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 2L, 5L, WILLBLOCK},
	{F_RDLCK, 0, 4L, 0L, SKIP, 0, 0L, 0L,
	 F_RDLCK, 0, 2L, 5L, NOBLOCK},
	{F_RDLCK, 0, 4L, 0L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 2L, 5L, WILLBLOCK},
	{F_WRLCK, 1, -3L, 2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 1L, 1L, NOBLOCK},
	{F_WRLCK, 1, -3L, 2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 4L, 1L, NOBLOCK},
	{F_WRLCK, 1, -3L, 2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 2L, 1L, WILLBLOCK},
	{F_WRLCK, 1, -3L, 2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 3L, 1L, WILLBLOCK},
	{F_WRLCK, 1, -3L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 1L, 1L, NOBLOCK},
	{F_WRLCK, 1, -3L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 1L, 1L, NOBLOCK},
	{F_WRLCK, 1, -3L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 2L, 1L, WILLBLOCK},
	{F_WRLCK, 1, -3L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 5L, 1L, WILLBLOCK},
	{F_WRLCK, 1, -3L, 5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 6L, 1L, WILLBLOCK},
	{F_WRLCK, 1, 2L, -2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 1L, 1L, NOBLOCK},
	{F_WRLCK, 1, -2L, -2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 4L, 1L, NOBLOCK},
	{F_WRLCK, 1, -2L, -2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 2L, 1L, WILLBLOCK},
	{F_WRLCK, 1, -2L, -2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 3L, 1L, WILLBLOCK},
	{F_WRLCK, 1, 2L, -2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 5L, 1L, NOBLOCK},
	{F_WRLCK, 1, 2L, -2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 8L, 1L, NOBLOCK},
	{F_WRLCK, 1, 2L, -2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 6L, 1L, WILLBLOCK},
	{F_WRLCK, 1, 2L, -2L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 7L, 1L, WILLBLOCK},
	{F_WRLCK, 1, 2L, -5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 2L, 1L, NOBLOCK},
	{F_WRLCK, 1, 2L, -5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 8L, 1L, NOBLOCK},
	{F_WRLCK, 1, 2L, -5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 3L, 1L, WILLBLOCK},
	{F_WRLCK, 1, 2L, -5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 5L, 1L, WILLBLOCK},
	{F_WRLCK, 1, 2L, -5L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 7L, 1L, WILLBLOCK},
	{F_WRLCK, 0, 0L, 0L, SKIP, 0, 0L, 0L,
	 F_WRLCK, 0, 15L, 0L, WILLBLOCK},
};

testcase *thiscase;

struct flock flock;

int parent, child, status, fail = 0;

int got1 = 0;

int fd;

int test;

char tmpname[40];
#define FILEDATA	"ten bytes!"
void catch1(int sig);
void catch_alarm(int sig);
char *TCID = "fcntl14";
int TST_TOTAL = 1;
int NO_NFS = 1;
#ifdef UCLINUX

char *argv0;
#endif
void cleanup(void)
{
	tst_rmdir();
}
void setup(void)
{
	struct sigaction act;
	signal(SIGHUP, SIG_IGN);
	umask(0);
	parent = getpid();
	sprintf(tmpname, "fcntl2.%d", parent);
	memset(&act, 0, sizeof(act));
	act.sa_handler = catch1;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGUSR1);
	if ((sigaction(SIGUSR1, &act, NULL)) < 0) {
		tst_resm(TFAIL, "SIGUSR1 signal setup failed, errno = %d",
			 errno);
		cleanup();
	}
	memset(&act, 0, sizeof(act));
	act.sa_handler = catch_alarm;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGALRM);
	if ((sigaction(SIGALRM, &act, NULL)) < 0) {
		tst_resm(TFAIL, "SIGALRM signal setup failed");
		cleanup();
	}
}
void wake_parent(void)
{
	if ((kill(parent, SIGUSR1)) < 0) {
		tst_resm(TFAIL, "Attempt to send signal to parent " "failed");
		tst_resm(TFAIL, "Test case %d, errno = %d", test + 1, errno);
		fail = 1;
	}
}
void do_usleep_child(void)
{
	wake_parent();
	exit(0);
}
void dochild(void)
{
	int rc;
	pid_t pid;
	flock.l_type = thiscase->c_type;
	flock.l_whence = thiscase->c_whence;
	flock.l_start = thiscase->c_start;
	flock.l_len = thiscase->c_len;
	flock.l_pid = 0;
	fail = 0;
	if ((rc = fcntl(fd, F_GETLK, &flock)) < 0) {
		tst_resm(TFAIL, "Attempt to check lock status failed");
		tst_resm(TFAIL, "Test case %d, errno = %d", test + 1, errno);
		fail = 1;
	} else {
		if ((thiscase->c_flag) == NOBLOCK) {
			if (flock.l_type != F_UNLCK) {
				tst_resm(TFAIL,
					 "Test case %d, GETLK: type = %d, "
					 "%d was expected", test + 1,
					 flock.l_type, F_UNLCK);
				fail = 1;
			}
			if (flock.l_whence != thiscase->c_whence) {
				tst_resm(TFAIL,
					 "Test case %d, GETLK: whence = %d, "
					 "should have remained %d", test + 1,
					 flock.l_whence, thiscase->c_whence);
				fail = 1;
			}
			if (flock.l_start != thiscase->c_start) {
				tst_resm(TFAIL,
					 "Test case %d, GETLK: start = %" PRId64
					 ", " "should have remained %" PRId64,
					 test + 1, (int64_t) flock.l_start,
					 (int64_t) thiscase->c_start);
				fail = 1;
			}
			if (flock.l_len != thiscase->c_len) {
				tst_resm(TFAIL,
					 "Test case %d, GETLK: len = %" PRId64
					 ", " "should have remained %" PRId64,
					 test + 1, (int64_t) flock.l_len,
					 (int64_t) thiscase->c_len);
				fail = 1;
			}
			if (flock.l_pid != 0) {
				tst_resm(TFAIL,
					 "Test case %d, GETLK: pid = %d, "
					 "should have remained 0", test + 1,
					 flock.l_pid);
				fail = 1;
			}
		} else {
			if (flock.l_pid != parent) {
				tst_resm(TFAIL,
					 "Test case %d, GETLK: pid = %d, "
					 "should be parent's id of %d",
					 test + 1, flock.l_pid, parent);
				fail = 1;
			}
			if (flock.l_type != thiscase->a_type) {
				tst_resm(TFAIL,
					 "Test case %d, GETLK: type = %d, "
					 "should be parent's first lock type of %d",
					 test + 1, flock.l_type,
					 thiscase->a_type);
				fail = 1;
			}
		}
	}
	flock.l_type = thiscase->c_type;
	flock.l_whence = thiscase->c_whence;
	flock.l_start = thiscase->c_start;
	flock.l_len = thiscase->c_len;
	flock.l_pid = 0;
	if ((rc = fcntl(fd, F_SETLK, &flock)) < 0) {
		if ((thiscase->c_flag) == NOBLOCK) {
			tst_resm(TFAIL, "Attempt to set child NONBLOCKING "
				 "lock failed");
			tst_resm(TFAIL, "Test case %d, errno = %d",
				 test + 1, errno);
			fail = 1;
		}
	}
	if ((thiscase->c_flag) == WILLBLOCK) {
		if (rc != -1 || (errno != EACCES && errno != EAGAIN)) {
			tst_resm(TFAIL,
				 "SETLK: rc = %d, errno = %d, -1/EAGAIN or EACCES "
				 "was expected", rc, errno);
			fail = 1;
		}
		if (rc == 0) {
			(void)fcntl(fd, F_UNLCK, &flock);
		}
		pid = FORK_OR_VFORK();
		switch (pid) {
		case -1:
			tst_resm(TFAIL, "Fork failed");
			fail = 1;
			break;
		case 0:
#ifdef UCLINUX
			if (self_exec(argv0, "nd", 1, parent) < 0) {
				tst_resm(TFAIL, "self_exec failed");
				break;
			}
#else
			do_usleep_child();
#endif
			break;
		default:
			if ((rc = fcntl(fd, F_SETLKW, &flock)) < 0) {
				tst_resm(TFAIL, "Attempt to set child BLOCKING "
					 "lock failed");
				tst_resm(TFAIL, "Test case %d, errno = %d",
					 test + 1, errno);
				fail = 1;
			}
			waitpid(pid, &status, 0);
			break;
		}
	}
	if (fail) {
		exit(1);
	} else {
		exit(0);
	}
}
void run_test(int file_flag, int file_mode, int seek, int start, int end)
{
	fail = 0;
	for (test = start; test < end; test++) {
		fd = open(cleanup, tmpname, file_flag, file_mode);
		if (write(fd, FILEDATA, 10) < 0)
			tst_brkm(TBROK, cleanup, "write() failed");
		if (seek) {
			lseek(cleanup, fd, seek, 0);
		}
		thiscase = &testcases[test];
		flock.l_type = thiscase->a_type;
		flock.l_whence = thiscase->a_whence;
		flock.l_start = thiscase->a_start;
		flock.l_len = thiscase->a_len;
		if ((fcntl(fd, F_SETLK, &flock)) < 0) {
			tst_resm(TFAIL, "First parent lock failed");
			tst_resm(TFAIL, "Test case %d, errno = %d",
				 test + 1, errno);
			fail = 1;
		}
		if ((thiscase->b_type) != SKIP) {
			flock.l_type = thiscase->b_type;
			flock.l_whence = thiscase->b_whence;
			flock.l_start = thiscase->b_start;
			flock.l_len = thiscase->b_len;
			if ((fcntl(fd, F_SETLK, &flock)) < 0) {
				tst_resm(TFAIL, "Second parent lock failed");
				tst_resm(TFAIL, "Test case %d, errno = %d",
					 test + 1, errno);
				fail = 1;
			}
		}
		if ((thiscase->c_type) == SKIP) {
			close(fd);
			tst_resm(TINFO, "skipping test %d", test + 1);
			continue;
		}
		(void)sighold(SIGUSR1);
		fflush(stdout);
		if ((child = FORK_OR_VFORK()) == 0) {
#ifdef UCLINUX
			if (self_exec(argv0, "nddddddddd", 2, thiscase->c_type,
				      thiscase->c_whence, thiscase->c_start,
				      thiscase->c_len, thiscase->c_flag,
				      thiscase->a_type, fd, test, parent) < 0) {
				tst_resm(TFAIL, "self_exec failed");
				cleanup();
			}
#else
			dochild();
#endif
		}
		if (child < 0)
			tst_brkm(TBROK|TERRNO, cleanup, "Fork failed");
		if ((thiscase->c_flag) == WILLBLOCK) {
			alarm(TIME_OUT);
			sigpause(SIGUSR1);
			alarm((unsigned)0);
			if (got1 != 1)
				tst_resm(TINFO, "Pause terminated without "
					 "signal SIGUSR1 from child");
			got1 = 0;
			 * setup lock structure for parent to delete
			 * blocking lock then wait for child to exit
			 */
			flock.l_type = F_UNLCK;
			flock.l_whence = 0;
			flock.l_start = 0L;
			flock.l_len = 0L;
			if ((fcntl(fd, F_SETLK, &flock)) < 0) {
				tst_resm(TFAIL, "Attempt to release parent "
					 "lock failed");
				tst_resm(TFAIL, "Test case %d, errno = %d",
					 test + 1, errno);
				fail = 1;
			}
		}
		alarm(TIME_OUT);
		waitpid(child, &status, 0);
		alarm((unsigned)0);
		if (status != 0) {
			tst_resm(TFAIL, "tchild returned status 0x%x", status);
			fail = 1;
		}
		close(fd);
		if (fail)
			tst_resm(TFAIL, "testcase:%d FAILED", test + 1);
		else
			tst_resm(TPASS, "testcase:%d PASSED", test + 1);
	}
	unlink(tmpname);
}
void catch_alarm(int sig)
{
	if (kill(child, 0) == 0) {
		kill(child, SIGKILL);
		perror("The child didnot terminate on its own accord");
	}
}
void catch1(int sig)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = catch1;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGUSR1);
	sigaction(SIGUSR1, &act, NULL);
	got1++;
}

void testcheck_end(int check_fail, char *msg)
{
	if (check_fail)
		tst_resm(TFAIL, "%s FAILED", msg);
	else
		tst_resm(TPASS, "%s PASSED", msg);
}
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	argv0 = av[0];
	maybe_run_child(&do_usleep_child, "nd", 1, &parent);
	thiscase = malloc(sizeof(testcase));
	maybe_run_child(&dochild, "nddddddddd", 2, &thiscase->c_type,
			&thiscase->c_whence, &thiscase->c_start,
			&thiscase->c_len, &thiscase->c_flag, &thiscase->a_type,
			&fd, &test, &parent);
#endif
	setup();
	if (tst_fs_type(cleanup, ".") == TST_NFS_MAGIC)
		NO_NFS = 0;
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		tst_resm(TINFO, "Enter block 1: without mandatory locking");
		fail = 0;
		(void)run_test(O_CREAT | O_RDWR | O_TRUNC, 0777, 0, 0, 36);
		testcheck_end(fail, "Block 1, test 1");
		(void)run_test(O_CREAT | O_RDWR | O_TRUNC, 0777, 5, 36, 45);
		testcheck_end(fail, "Block 1, test 2");
		tst_resm(TINFO, "Exit block 1");
		tst_resm(TINFO, "Enter block 2: with mandatory locking");
		if (NO_NFS) {
			fail = 0;
			(void)run_test(O_CREAT | O_RDWR | O_TRUNC,
				S_ENFMT | S_IRUSR | S_IWUSR, 0, 0, 36);
			testcheck_end(fail, "Block 2, test 1");
			(void)run_test(O_CREAT | O_RDWR | O_TRUNC,
				S_ENFMT | S_IRUSR | S_IWUSR, 5, 36, 45);
			testcheck_end(fail, "Block 2, test 2");
		} else {
			tst_resm(TCONF, "Skip block 2 as NFS does not"
				" support mandatory locking");
		}
		tst_resm(TINFO, "Exit block 2");
		tst_resm(TINFO, "Enter block 3");
		fail = 0;
		fd = open(cleanup, tmpname, O_CREAT | O_RDWR | O_TRUNC,
			       0777);
		if (write(fd, FILEDATA, 10) < 0)
			tst_brkm(TBROK, cleanup, "write failed");
		flock.l_type = F_WRLCK;
		flock.l_whence = -1;
		flock.l_start = 0L;
		flock.l_len = 0L;
		if ((fcntl(fd, F_SETLK, &flock)) < 0) {
			if (errno != EINVAL) {
				tst_resm(TFAIL, "Expected %d got %d",
					 EINVAL, errno);
				fail = 1;
			}
		} else {
			tst_resm(TFAIL, "Lock succeeded when it should have "
				 "failed");
			fail = 1;
		}
		close(fd);
		unlink(tmpname);
		testcheck_end(fail, "Test with negative whence locking");
		tst_resm(TINFO, "Exit block 3");
		tst_resm(TINFO, "Enter block 4");
		fail = 0;
		fd = open(cleanup, tmpname, O_CREAT | O_RDWR | O_TRUNC,
			       0777);
		if (write(fd, FILEDATA, 10) < 0)
			tst_brkm(TBROK, cleanup, "write failed");
		thiscase = &testcases[58];
		flock.l_type = thiscase->a_type;
		flock.l_whence = thiscase->a_whence;
		flock.l_start = thiscase->a_start;
		flock.l_len = thiscase->a_len;
		if ((fcntl(fd, F_SETLK, &flock)) < 0) {
			tst_resm(TFAIL, "First parent lock failed");
			tst_resm(TFAIL, "Test case %d, errno = %d", 58, errno);
			fail = 1;
		}
		if (write(fd, FILEDATA, 10) < 0)
			tst_brkm(TBROK, cleanup, "write failed");
		if (sighold(SIGUSR1) < 0)
			tst_brkm(TBROK, cleanup, "sighold failed");
		if ((child = FORK_OR_VFORK()) == 0) {
#ifdef UCLINUX
			if (self_exec(argv0, "nddddddddd", 2, thiscase->c_type,
				      thiscase->c_whence, thiscase->c_start,
				      thiscase->c_len, thiscase->c_flag,
				      thiscase->a_type, fd, test, parent) < 0) {
				tst_resm(TFAIL, "self_exec failed");
				cleanup();
			}
#else
			dochild();
#endif
		}
		if (child < 0)
			tst_brkm(TBROK|TERRNO, cleanup, "Fork failed");
		(void)alarm(TIME_OUT);
		(void)sigpause(SIGUSR1);
		(void)alarm((unsigned)0);
		if (got1 != 1) {
			tst_resm(TINFO, "Pause terminated without signal "
				 "SIGUSR1 from child");
		}
		got1 = 0;
		flock.l_type = F_UNLCK;
		flock.l_whence = 0;
		flock.l_start = 0L;
		flock.l_len = 0L;
		if ((fcntl(fd, F_SETLK, &flock)) < 0) {
			tst_resm(TFAIL, "Attempt to release parent lock "
				 "failed");
			tst_resm(TFAIL, "Test case %d, errno = %d", test + 1,
				 errno);
			fail = 1;
		}
		(void)alarm(TIME_OUT);
		waitpid(child, &status, 0);
		if (WEXITSTATUS(status) != 0) {
			fail = 1;
			tst_resm(TFAIL, "child returned bad exit status");
		}
		(void)alarm((unsigned)0);
		if (status != 0) {
			tst_resm(TFAIL, "child returned status 0x%x", status);
			fail = 1;
		}
		close(fd);
		unlink(tmpname);
		testcheck_end(fail, "Test of locks on file");
		tst_resm(TINFO, "Exit block 4");
	}
	cleanup();
	tst_exit();
}

