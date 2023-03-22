#include "incl.h"
#define SKIPVAL 0x0f00
#define SKIP 0,0,0L,0L,0
#if (SKIPVAL == F_RDLCK) || (SKIPVAL == F_WRLCK)
#error invalid SKIP, must not be F_RDLCK or F_WRLCK
#endif
#define	IGNORED		0
#define	TIME_OUT	10
typedef struct {
	struct flock parent_a;
	struct flock parent_b;
	struct flock child_a;
	struct flock child_b;
	struct flock parent_c;
	struct flock parent_d;
} testcase;

testcase testcases[] = {
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},
	 {F_RDLCK, 0, 0L, 5L, NOBLOCK},
	 {F_RDLCK, 0, 6L, 5L, NOBLOCK},
	  * Parent_c read lock on entire file
	  */
	 {F_RDLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},},
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},
	 {F_RDLCK, 0, 0L, 5L, WILLBLOCK},
	 {F_RDLCK, 0, 6L, 5L, WILLBLOCK},
	  * Parent_c write lock on entire
	  * file
	  */
	 {F_WRLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},},
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},
	 {F_RDLCK, 0, 2L, 3L, WILLBLOCK},
	 {F_RDLCK, 0, 6L, 3L, WILLBLOCK},
	  * Parent_c read lock on byte 3 to
	  * byte 7
	  */
	 {F_RDLCK, 0, 3L, 5L, IGNORED},
	 {SKIP},},
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},
	 {F_RDLCK, 0, 2L, 3L, WILLBLOCK},
	 {F_RDLCK, 0, 6L, 3L, NOBLOCK},
	  * Parent_c read lock on byte 5 to
	  * byte 9
	  */
	 {F_RDLCK, 0, 5L, 5L, IGNORED},
	 {SKIP},},
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},
	 {F_RDLCK, 0, 3L, 5L, NOBLOCK},
	 {F_RDLCK, 0, 5L, 6L, WILLBLOCK},
	  * Parent_c read lock on byte 2 to
	  * byte 8
	  */
	 {F_RDLCK, 0, 2L, 7L, IGNORED},
	 {SKIP},},
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},
	 {F_RDLCK, 0, 2L, 3L, WILLBLOCK},
	 {F_RDLCK, 0, 6L, 3L, NOBLOCK},
	 {F_UNLCK, 0, 3L, 7L, IGNORED},
	 {SKIP},},
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {F_RDLCK, 0, 3L, 5L, IGNORED},
	 {F_RDLCK, 0, 2L, 3L, NOBLOCK},
	 {F_RDLCK, 0, 6L, 3L, NOBLOCK},
	  * Parent_c read lock on byte 1 to
	  * byte 9
	  */
	 {F_RDLCK, 0, 1L, 9L, IGNORED},
	 {SKIP},},
	{{F_WRLCK, 0, 2L, 3L, IGNORED},
	 {F_WRLCK, 0, 6L, 3L, IGNORED},
	 {F_RDLCK, 0, 3L, 5L, NOBLOCK},
	 {SKIP},
	  * Parent_c read lock on byte 1 to
	  * byte 5
	  */
	 {F_RDLCK, 0, 1L, 5L, IGNORED},
	  * Parent_d read lock on
	  * byte 5 to byte 9
	  */
	 {F_RDLCK, 0, 5L, 5L,
	  IGNORED},},
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {F_RDLCK, 0, 3L, 5L, IGNORED},
	 {F_RDLCK, 0, 2L, 3L, NOBLOCK},
	 {F_RDLCK, 0, 6L, 3L, NOBLOCK},
	  * Parent_c read lock on byte 1 to
	  * byte 3
	  */
	 {F_RDLCK, 0, 1L, 3L, IGNORED},
	  * Parent_d read lock on
	  * byte 7 to byte 9
	  */
	 {F_RDLCK, 0, 7L, 3L,
	  IGNORED},},
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},
	 {F_RDLCK, 0, 2L, 3L, NOBLOCK},
	 {F_RDLCK, 0, 6L, 3L, NOBLOCK},
	  * Parent_c read lock on byte 1 to
	  * byte 7
	  */
	 {F_RDLCK, 0, 1L, 7L, IGNORED},
	  * Parent_d read lock on
	  * byte 3 to byte 9
	  */
	 {F_RDLCK, 0, 3L, 7L,
	  IGNORED},},
	{{F_WRLCK, 0, 0L, 0L, IGNORED},
	 {SKIP},
	 {F_RDLCK, 0, 3L, 5L, NOBLOCK},
	 {F_RDLCK, 0, 3L, 5L, NOBLOCK},
	  * Parent_c read lock on byte 3 to
	  * byte 7
	  */
	 {F_RDLCK, 0, 3L, 5L, IGNORED},
	 {SKIP},},
};

testcase *thiscase;

struct flock *thislock;

int parent;

int child_flag1 = 0;

int child_flag2 = 0;

int parent_flag = 0;

int alarm_flag = 0;

int child_pid[2], flag[2];

int fd;

int test;

char tmpname[40];
#define	FILEDATA	"tenbytes!"
char *TCID = "fcntl16";
int TST_TOTAL = 1;
#ifdef UCLINUX

char *argv0;
#endif
void cleanup(void)
{
	tst_rmdir();
}
void dochild(int kid)
{
	struct sigaction sact;
	sact.sa_flags = 0;
	sact.sa_handler = catch_int;
	sigemptyset(&sact.sa_mask);
	(void)sigaction(SIGUSR1, &sact, NULL);
	if (kid) {
		if ((kill(parent, SIGUSR2)) < 0) {
			tst_resm(TFAIL, "Attempt to send signal to parent "
				 "failed");
			tst_resm(TFAIL, "Test case %d, child %d, errno = %d",
				 test + 1, kid, errno);
			exit(1);
		}
	} else {
		if ((kill(parent, SIGUSR1)) < 0) {
			tst_resm(TFAIL, "Attempt to send signal to parent "
				 "failed");
			tst_resm(TFAIL, "Test case %d, child %d, errno = %d",
				 test + 1, kid, errno);
			exit(1);
		}
	}
	if ((fcntl(fd, F_SETLKW, thislock)) < 0) {
		if (errno == EINTR && parent_flag) {
			 * signal received is waiting for lock to clear,
			 * this is expected if flag = WILLBLOCK
			 */
			exit(1);
		} else {
			tst_resm(TFAIL, "Attempt to set child BLOCKING lock "
				 "failed");
			tst_resm(TFAIL, "Test case %d, errno = %d", test + 1,
				 errno);
			exit(2);
		}
	}
	exit(0);
#ifdef UCLINUX

int kid_uc;
void dochild_uc(void)
{
	dochild(kid_uc);
}
#endif
void catch_alarm(int sig)
{
	alarm_flag = 1;
}
void catch_usr1(int sig)
	 * Set flag to let parent know that child #1 is ready to have the
	 * lock removed
	 */
	child_flag1 = 1;
}
void catch_usr2(int sig)
	 * Set flag to let parent know that child #2 is ready to have the
	 * lock removed
	 */
	child_flag2 = 1;
}
void catch_int(int sig)
	 * Set flag to interrupt fcntl call in child and force a controlled
	 * exit
	 */
	parent_flag = 1;
}
void child_sig(int sig, int nkids)
{
	int i;
	for (i = 0; i < nkids; i++) {
		if (kill(child_pid[i], 0) == 0) {
			if ((kill(child_pid[i], sig)) < 0) {
				tst_resm(TFAIL, "Attempt to signal child %d, "
					 "failed", i + 1);
			}
		}
	}
}
void setup(void)
{
	struct sigaction sact;
	tst_sig(FORK, DEF_HANDLER, cleanup);
	umask(0);
	TEST_PAUSE;
	parent = getpid();
	tst_tmpdir();
	if (tst_fs_type(cleanup, ".") == TST_NFS_MAGIC)
		NO_NFS = 0;
	sprintf(tmpname, "fcntl4.%d", parent);
	 * Set up signal handling functions
	 */
	memset(&sact, 0, sizeof(sact));
	sact.sa_handler = catch_usr1;
	sigemptyset(&sact.sa_mask);
	sigaddset(&sact.sa_mask, SIGUSR1);
	sigaction(SIGUSR1, &sact, NULL);
	memset(&sact, 0, sizeof(sact));
	sact.sa_handler = catch_usr2;
	sigemptyset(&sact.sa_mask);
	sigaddset(&sact.sa_mask, SIGUSR2);
	sigaction(SIGUSR2, &sact, NULL);
	memset(&sact, 0, sizeof(sact));
	sact.sa_handler = catch_alarm;
	sigemptyset(&sact.sa_mask);
	sigaddset(&sact.sa_mask, SIGALRM);
	sigaction(SIGALRM, &sact, NULL);
}
int run_test(int file_flag, int file_mode, int start, int end)
{
	int child_count;
	int child;
	int nexited;
	int status, expect_stat;
	int i, fail = 0;
	for (test = start; test < end; test++) {
		fd = open(cleanup, tmpname, file_flag, file_mode);
		(void)write(fd, FILEDATA, 10);
		thiscase = &testcases[test];
		thislock = &thiscase->parent_a;
		if ((fcntl(fd, F_SETLK, thislock)) < 0) {
			tst_resm(TFAIL, "First parent lock failed");
			tst_resm(TFAIL, "Test case %d, errno = %d", test + 1,
				 errno);
			close(fd);
			unlink(tmpname);
			return 1;
		}
		thislock = &thiscase->parent_b;
			if ((fcntl(fd, F_SETLK, thislock)) < 0) {
				tst_resm(TFAIL, "Second parent lock failed");
				tst_resm(TFAIL, "Test case %d, errno = %d",
					 test + 1, errno);
				close(fd);
				unlink(tmpname);
				return 1;
			}
		}
		thislock = &thiscase->child_a;
		alarm_flag = parent_flag = 0;
		child_flag1 = child_flag2 = 0;
		child_count = 0;
		for (i = 0; i < 2; i++) {
			if (thislock->l_type != IGNORED) {
				if ((child = FORK_OR_VFORK()) == 0) {
#ifdef UCLINUX
					if (self_exec(argv0, "ddddd", i, parent,
						      test, thislock, fd) < 0) {
						perror("self_exec failed");
						return 1;
					}
#else
					dochild(i);
#endif
				}
				if (child < 0) {
					perror("Fork failed");
					return 1;
				}
				child_count++;
				child_pid[i] = child;
				flag[i] = thislock->l_pid;
			}
			thislock = &thiscase->child_b;
		}
		 * Wait for children to signal they are ready. Set a timeout
		 * just in case they don't signal at all.
		 */
		alarm(TIME_OUT);
		while (!alarm_flag
		       && (child_flag1 + child_flag2 != child_count)) {
			pause();
		}
		 * Turn off alarm and unmask signals
		 */
		alarm((unsigned)0);
		if (child_flag1 + child_flag2 != child_count) {
			tst_resm(TFAIL, "Test case %d: kids didn't signal",
				 test + 1);
			fail = 1;
		}
		child_flag1 = child_flag2 = alarm_flag = 0;
		thislock = &thiscase->parent_c;
		if ((fcntl(fd, F_SETLK, thislock)) < 0) {
			tst_resm(TFAIL, "Third parent lock failed");
			tst_resm(TFAIL, "Test case %d, errno = %d",
				 test + 1, errno);
			close(fd);
			unlink(tmpname);
			return 1;
		}
		thislock = &thiscase->parent_d;
			if ((fcntl(fd, F_SETLK, thislock)) < 0) {
				tst_resm(TINFO, "Fourth parent lock failed");
				tst_resm(TINFO, "Test case %d, errno = %d",
					 test + 1, errno);
				close(fd);
				unlink(tmpname);
				return 1;
			}
		}
		 * Wait for children to exit, or for timeout to occur.
		 * Timeouts are expected for testcases where kids are
		 * 'WILLBLOCK', In that case, send kids a wakeup interrupt
		 * and wait again for them. If a second timeout occurs, then
		 * something is wrong.
		 */
		alarm_flag = nexited = 0;
		while (nexited < child_count) {
			alarm(TIME_OUT);
			child = wait(&status);
			alarm(0);
			if (child == -1) {
				if (errno != EINTR || alarm_flag != 1) {
					 * Some error other than a timeout,
					 * or else this is the second
					 * timeout. Both cases are errors.
					 */
					break;
				}
				 * Expected timeout case. Signal kids then
				 * go back and wait again
				 */
				child_sig(SIGUSR1, child_count);
				continue;
			}
			for (i = 0; i < child_count; i++)
				if (child == child_pid[i])
					break;
			if (i == child_count) {
				 * Ignore unexpected kid, it could be a
				 * leftover from a previous iteration that
				 * timed out.
				 */
				continue;
			}
			nexited++;
			expect_stat = (flag[i] == NOBLOCK) ? 0 : 1;
			if (!WIFEXITED(status)
			    || WEXITSTATUS(status) != expect_stat) {
				tst_resm(TFAIL, "Test case %d: child %d %s "
					 "or got bad status (x%x)", test + 1,
					 i, (flag[i] == NOBLOCK) ?
					 "BLOCKED unexpectedly" :
					 "failed to BLOCK", status);
				fail = 1;
			}
		}
		if (nexited != child_count) {
			tst_resm(TFAIL, "Test case %d, caught %d expected %d "
				 "children", test + 1, nexited, child_count);
			child_sig(SIGKILL, nexited);
			fail = 1;
		}
		close(fd);
	}
	unlink(tmpname);
	if (fail) {
		return 1;
	} else {
		return 0;
	}
	return 0;
}
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(dochild_uc, "ddddd", &kid_uc, &parent, &test,
			&thislock, &fd);
	argv0 = av[0];
#endif
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Check file locks on an ordinary file without
		 * mandatory locking
		 */
		tst_resm(TINFO, "Entering block 1");
		if (run_test(O_CREAT | O_RDWR | O_TRUNC, 0777, 0, 11)) {
			tst_resm(TINFO, "Test case 1: without mandatory "
				 "locking FAILED");
		} else {
			tst_resm(TINFO, "Test case 1: without manadatory "
				 "locking PASSED");
		}
		tst_resm(TINFO, "Exiting block 1");
		 * Check the file locks on a file with mandatory record
		 * locking
		 */
		tst_resm(TINFO, "Entering block 2");
		if (NO_NFS && run_test(O_CREAT | O_RDWR | O_TRUNC, S_ISGID |
			     S_IRUSR | S_IWUSR, 0, 11)) {
			tst_resm(TINFO, "Test case 2: with mandatory record "
				 "locking FAILED");
		} else {
			if (NO_NFS)
				tst_resm(TINFO, "Test case 2: with mandatory"
					 " record locking PASSED");
			else
				tst_resm(TCONF, "Test case 2: NFS does not"
					 " support mandatory locking");
		}
		tst_resm(TINFO, "Exiting block 2");
		 * Check file locks on a file with mandatory record locking
		 * and no delay
		 */
		tst_resm(TINFO, "Entering block 3");
		if (NO_NFS && run_test(O_CREAT | O_RDWR | O_TRUNC | O_NDELAY,
			     S_ISGID | S_IRUSR | S_IWUSR, 0, 11)) {
			tst_resm(TINFO, "Test case 3: mandatory locking with "
				 "NODELAY FAILED");
		} else {
			if (NO_NFS)
				tst_resm(TINFO, "Test case 3: mandatory"
					 " locking with NODELAY PASSED");
			else
				tst_resm(TCONF, "Test case 3: NFS does not"
					 " support mandatory locking");
		}
		tst_resm(TINFO, "Exiting block 3");
	}
	cleanup();
	tst_exit();
}

