#include "incl.h"
		fcntl05 => fcntl17, check signal return for SIG_ERR)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
char *TCID = "fcntl17";
int TST_TOTAL = 1;
#define STRINGSIZE	27
#define STRING		"abcdefghijklmnopqrstuvwxyz\n"
#define STOP		0xFFF0
#define TIME_OUT	10
int parent_pipe[2];
int child_pipe1[2];
int child_pipe2[2];
int child_pipe3[2];
int file_fd;
pid_t parent_pid, child_pid1, child_pid2, child_pid3;
int child_stat;
struct flock lock1 = { (short)F_WRLCK, (short)0, 2, 5, (short)0 };
struct flock lock2 = { (short)F_WRLCK, (short)0, 9, 5, (short)0 };
struct flock lock3 = { (short)F_WRLCK, (short)0, 17, 5, (short)0 };
struct flock lock4 = { (short)F_WRLCK, (short)0, 17, 5, (short)0 };
struct flock lock5 = { (short)F_WRLCK, (short)0, 2, 14, (short)0 };
struct flock unlock = { (short)F_UNLCK, (short)0, 0, 0, (short)0 };
int setup();
void cleanup();
int parent_wait();
void parent_free();
void child_wait();
void child_free();
void do_child1();
void do_child2();
void do_child3();
int do_test(struct flock *, pid_t);
void stop_children();
void catch_child();
void catch_alarm();
char *str_type();
int setup(void)
{
	char *buf = STRING;
	char template[PATH_MAX];
	struct sigaction act;
	tst_sig(FORK, DEF_HANDLER, NULL);
	umask(0);
	TEST_PAUSE;
	if (pipe(parent_pipe) < 0) {
		tst_resm(TFAIL, "Couldn't create parent_pipe! errno = %d",
			 errno);
		return 1;
	}
	if (pipe(child_pipe1) < 0) {
		tst_resm(TFAIL, "Couldn't create child_pipe1! errno = %d",
			 errno);
		return 1;
	}
	if (pipe(child_pipe2) < 0) {
		tst_resm(TFAIL, "Couldn't create child_pipe2! errno = %d",
			 errno);
		return 1;
	}
	if (pipe(child_pipe3) < 0) {
		tst_resm(TFAIL, "Couldn't create child_pipe3! errno = %d",
			 errno);
		return 1;
	}
	parent_pid = getpid();
	snprintf(template, PATH_MAX, "fcntl17XXXXXX");
	if ((file_fd = mkstemp(template)) < 0) {
		tst_resm(TFAIL, "Couldn't open temp file! errno = %d", errno);
	}
	if (write(file_fd, buf, STRINGSIZE) < 0) {
		tst_resm(TFAIL, "Couldn't write to temp file! errno = %d",
			 errno);
	}
	memset(&act, 0, sizeof(act));
	act.sa_handler = catch_alarm;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGALRM);
	if (sigaction(SIGALRM, &act, NULL) < 0) {
		tst_resm(TFAIL, "SIGALRM signal setup failed, errno: %d",
			 errno);
		return 1;
	}
	memset(&act, 0, sizeof(act));
	act.sa_handler = catch_child;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGCHLD);
	if (sigaction(SIGCHLD, &act, NULL) < 0) {
		tst_resm(TFAIL, "SIGCHLD signal setup failed, errno: %d", errno);
		return 1;
	}
	return 0;
}
void cleanup(void)
{
	if (child_pid1 > 0)
		kill(child_pid1, 9);
	if (child_pid2 > 0)
		kill(child_pid2, 9);
	if (child_pid3 > 0)
		kill(child_pid3, 9);
	close(file_fd);
	tst_rmdir();
}
void do_child1(void)
{
	int err;
	close(parent_pipe[0]);
	close(child_pipe1[1]);
	close(child_pipe2[0]);
	close(child_pipe2[1]);
	close(child_pipe3[0]);
	close(child_pipe3[1]);
	child_wait(child_pipe1[0]);
	tst_resm(TINFO, "child 1 starting");
	if (fcntl(file_fd, F_SETLK, &lock1) < 0) {
		err = errno;
		tst_resm(TINFO, "child 1 lock err %d", err);
		parent_free(err);
	} else {
		tst_resm(TINFO, "child 1 pid %d locked", getpid());
		parent_free(0);
	}
	child_wait(child_pipe1[0]);
	tst_resm(TINFO, "child 1 resuming");
	fcntl(file_fd, F_SETLK, &unlock);
	tst_resm(TINFO, "child 1 unlocked");
	child_wait(child_pipe1[0]);
	tst_resm(TINFO, "child 1 exiting");
	exit(1);
}
void do_child2(void)
{
	int err;
	close(parent_pipe[0]);
	close(child_pipe1[0]);
	close(child_pipe1[1]);
	close(child_pipe2[1]);
	close(child_pipe3[0]);
	close(child_pipe3[1]);
	child_wait(child_pipe2[0]);
	tst_resm(TINFO, "child 2 starting");
	if (fcntl(file_fd, F_SETLK, &lock2) < 0) {
		err = errno;
		tst_resm(TINFO, "child 2 lock err %d", err);
		parent_free(err);
	} else {
		tst_resm(TINFO, "child 2 pid %d locked", getpid());
		parent_free(0);
	}
	child_wait(child_pipe2[0]);
	tst_resm(TINFO, "child 2 resuming");
	if (fcntl(file_fd, F_SETLKW, &lock4) < 0) {
		err = errno;
		tst_resm(TINFO, "child 2 lockw err %d", err);
		parent_free(err);
	} else {
		tst_resm(TINFO, "child 2 lockw locked");
		parent_free(0);
	}
	child_wait(child_pipe2[0]);
	tst_resm(TINFO, "child 2 exiting");
	exit(1);
}
void do_child3(void)
{
	int err;
	close(parent_pipe[0]);
	close(child_pipe1[0]);
	close(child_pipe1[1]);
	close(child_pipe2[0]);
	close(child_pipe2[1]);
	close(child_pipe3[1]);
	child_wait(child_pipe3[0]);
	tst_resm(TINFO, "child 3 starting");
	if (fcntl(file_fd, F_SETLK, &lock3) < 0) {
		err = errno;
		tst_resm(TINFO, "child 3 lock err %d", err);
		parent_free(err);
	} else {
		tst_resm(TINFO, "child 3 pid %d locked", getpid());
		parent_free(0);
	}
	child_wait(child_pipe3[0]);
	tst_resm(TINFO, "child 3 resuming");
	if (fcntl(file_fd, F_SETLKW, &lock5) < 0) {
		err = errno;
		tst_resm(TINFO, "child 3 lockw err %d", err);
		parent_free(err);
	} else {
		tst_resm(TINFO, "child 3 lockw locked");
		parent_free(0);
	}
	child_wait(child_pipe3[0]);
	tst_resm(TINFO, "child 3 exiting");
	exit(1);
}
int do_test(struct flock *lock, pid_t pid)
{
	struct flock fl;
	fl.l_whence = lock->l_whence;
	fl.l_start = lock->l_start;
	fl.l_len = lock->l_len;
	fl.l_pid = (short)0;
	if (fcntl(file_fd, F_GETLK, &fl) < 0) {
		tst_resm(TFAIL, "fcntl on file failed, errno =%d", errno);
		return 1;
	}
	if (fl.l_type != lock->l_type) {
		tst_resm(TFAIL, "lock type is wrong should be %s is %s",
			 str_type(lock->l_type), str_type(fl.l_type));
		return 1;
	}
	if (fl.l_whence != lock->l_whence) {
		tst_resm(TFAIL, "lock whence is wrong should be %d is %d",
			 lock->l_whence, fl.l_whence);
		return 1;
	}
	if (fl.l_start != lock->l_start) {
		tst_resm(TFAIL, "region starts in wrong place, "
			 "should be %" PRId64 " is %" PRId64,
			 (int64_t) lock->l_start, (int64_t) fl.l_start);
		return 1;
	}
	if (fl.l_len != lock->l_len) {
		tst_resm(TFAIL,
			 "region length is wrong, should be %" PRId64 " is %"
			 PRId64, (int64_t) lock->l_len, (int64_t) fl.l_len);
		return 1;
	}
	if (fl.l_pid != pid) {
		tst_resm(TFAIL, "locking pid is wrong, should be %d is %d",
			 pid, fl.l_pid);
		return 1;
	}
	return 0;
}
char *str_type(int type)
{
	

char buf[20];
	switch (type) {
	case F_RDLCK:
		return ("F_RDLCK");
	case F_WRLCK:
		return ("F_WRLCK");
	case F_UNLCK:
		return ("F_UNLCK");
	default:
		sprintf(buf, "BAD VALUE: %d", type);
		return (buf);
	}
}
void parent_free(int arg)
{
	if (write(parent_pipe[1], &arg, sizeof(arg)) != sizeof(arg)) {
		tst_resm(TFAIL, "couldn't send message to parent");
		exit(1);
	}
}
int parent_wait(void)
{
	int arg;
	if (read(parent_pipe[0], &arg, sizeof(arg)) != sizeof(arg)) {
		tst_resm(TFAIL, "parent_wait() failed");
		return (errno);
	}
	return (arg);
}
void child_free(int fd, int arg)
{
	if (write(fd, &arg, sizeof(arg)) != sizeof(arg)) {
		tst_resm(TFAIL, "couldn't send message to child");
		exit(1);
	}
}
void child_wait(int fd)
{
	int arg;
	if (read(fd, &arg, sizeof(arg)) != sizeof(arg)) {
		tst_resm(TFAIL, "couldn't get message from parent");
		exit(1);
	} else if (arg == (short)STOP) {
		exit(0);
	}
}
void stop_children(void)
{
	int arg;
	signal(SIGCHLD, SIG_DFL);
	arg = STOP;
	child_free(child_pipe1[1], arg);
	child_free(child_pipe2[1], arg);
	child_free(child_pipe3[1], arg);
	waitpid(child_pid1, &child_stat, 0);
	child_pid1 = 0;
	waitpid(child_pid2, &child_stat, 0);
	child_pid2 = 0;
	waitpid(child_pid3, &child_stat, 0);
	child_pid3 = 0;
}
void catch_child(void)
{
	tst_resm(TFAIL, "Unexpected death of child process");
	cleanup();
}
void catch_alarm(void)
{
	sighold(SIGCHLD);
	 * Timer has runout and the children have not detected the deadlock.
	 * Need to kill the kids and exit
	 */
	if (child_pid1 != 0 && (kill(child_pid1, SIGKILL)) < 0) {
		tst_resm(TFAIL, "Attempt to signal child 1 failed.");
	}
	if (child_pid2 != 0 && (kill(child_pid2, SIGKILL)) < 0) {
		tst_resm(TFAIL, "Attempt to signal child 2 failed.");
	}
	if (child_pid3 != 0 && (kill(child_pid3, SIGKILL)) < 0) {
		tst_resm(TFAIL, "Attempt to signal child 2 failed.");
	}
	tst_resm(TFAIL, "Alarm expired, deadlock not detected");
	tst_resm(TWARN, "You may need to kill child processes by hand");
	cleanup();
}
int main(int ac, char **av)
{
	int ans;
	int lc;
	int fail = 0;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&do_child1, "nddddddddd", 1, &file_fd,
			&parent_pipe[0], &parent_pipe[1],
			&child_pipe1[0], &child_pipe1[1],
			&child_pipe2[0], &child_pipe2[1],
			&child_pipe3[0], &child_pipe3[1]);
	maybe_run_child(&do_child2, "nddddddddd", 2, &file_fd,
			&parent_pipe[0], &parent_pipe[1],
			&child_pipe1[0], &child_pipe1[1],
			&child_pipe2[0], &child_pipe2[1],
			&child_pipe3[0], &child_pipe3[1]);
	maybe_run_child(&do_child3, "nddddddddd", 3, &file_fd,
			&parent_pipe[0], &parent_pipe[1],
			&child_pipe1[0], &child_pipe1[1],
			&child_pipe2[0], &child_pipe2[1],
			&child_pipe3[0], &child_pipe3[1]);
#endif
		tst_resm(TINFO, "setup failed");
		cleanup();
	}
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		tst_resm(TINFO, "Enter preparation phase");
#ifdef UCLINUX
			if (self_exec(av[0], "nddddddddd", 1, file_fd,
				      parent_pipe[0], parent_pipe[1],
				      child_pipe1[0], child_pipe1[1],
				      child_pipe2[0], child_pipe2[1],
				      child_pipe3[0], child_pipe3[1]) < 0) {
				perror("self_exec failed, child 1");
				cleanup();
			}
#else
			do_child1();
#endif
		} else if (child_pid1 < 0)
			tst_brkm(TBROK|TERRNO, cleanup, "Fork failed: child 1");
#ifdef UCLINUX
			if (self_exec(av[0], "nddddddddd", 2, file_fd,
				      parent_pipe[0], parent_pipe[1],
				      child_pipe1[0], child_pipe1[1],
				      child_pipe2[0], child_pipe2[1],
				      child_pipe3[0], child_pipe3[1]) < 0) {
				perror("self_exec failed, child 2");
				cleanup();
			}
#else
			do_child2();
#endif
		} else if (child_pid2 < 0) {
			tst_brkm(TBROK|TERRNO, cleanup, "Fork failed: child 2");
		}
#ifdef UCLINUX
			if (self_exec(av[0], "nddddddddd", 3, file_fd,
				      parent_pipe[0], parent_pipe[1],
				      child_pipe1[0], child_pipe1[1],
				      child_pipe2[0], child_pipe2[1],
				      child_pipe3[0], child_pipe3[1]) < 0) {
				perror("self_exec failed, child 3");
				cleanup();
			}
#else
			do_child3();
#endif
			do_child3();
		} else if (child_pid3 < 0) {
			tst_brkm(TBROK|TERRNO, cleanup, "Fork failed: child 3");
		}
		close(parent_pipe[1]);
		close(child_pipe1[0]);
		close(child_pipe2[0]);
		close(child_pipe3[0]);
		tst_resm(TINFO, "Exit preparation phase");
		tst_resm(TINFO, "Enter block 1");
		fail = 0;
		 * child 1 puts first lock (bytes 2-7)
		 */
		child_free(child_pipe1[1], 0);
		if (parent_wait()) {
			tst_resm(TFAIL, "didn't set first child's lock, "
				 "errno: %d", errno);
		}
		if (do_test(&lock1, child_pid1)) {
			tst_resm(TINFO, "do_test failed child 1");
			fail = 1;
		}
		 * child 2 puts second lock (bytes 9-14)
		 */
		child_free(child_pipe2[1], 0);
		if (parent_wait()) {
			tst_resm(TINFO, "didn't set second child's lock, "
				 "errno: %d", errno);
			fail = 1;
		}
		if (do_test(&lock2, child_pid2)) {
			tst_resm(TINFO, "do_test failed child 2");
			fail = 1;
		}
		 * child 3 puts third lock (bytes 17-22)
		 */
		child_free(child_pipe3[1], 0);
		if (parent_wait()) {
			tst_resm(TFAIL, "didn't set third child's lock, "
				 "errno: %d", errno);
			fail = 1;
		}
		if (do_test(&lock3, child_pid3)) {
			tst_resm(TINFO, "do_test failed child 3");
			fail = 1;
		}
		 * child 2 tries to lock same range as
		 * child 3's first lock.
		 */
		child_free(child_pipe2[1], 0);
		 * child 3 tries to lock same range as
		 * child 1 and child 2's first locks.
		 */
		child_free(child_pipe3[1], 0);
		 * Tell child 1 to release its lock. This should cause a
		 * delayed deadlock between child 2 and child 3.
		 */
		child_free(child_pipe1[1], 0);
		 * Setup an alarm to go off in case the deadlock is not
		 * detected
		 */
		alarm(TIME_OUT);
		 * should get a message from child 3 telling that its
		 * second lock EDEADLOCK
		 */
		if ((ans = parent_wait()) != EDEADLK) {
			tst_resm(TFAIL, "child 2 didn't deadlock, "
				 "returned: %d", ans);
			fail = 1;
		}
		 * Double check that lock 2 and lock 3 are still right
		 */
		do_test(&lock2, child_pid2);
		do_test(&lock3, child_pid3);
		stop_children();
		if (fail) {
			tst_resm(TFAIL, "Block 1 FAILED");
		} else {
			tst_resm(TPASS, "Block 1 PASSED");
		}
		tst_resm(TINFO, "Exit block 1");
	}
	cleanup();
	tst_exit();
}

