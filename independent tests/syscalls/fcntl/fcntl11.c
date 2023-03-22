#include "incl.h"
#define STRINGSIZE	27
#define STRING		"abcdefghijklmnopqrstuvwxyz\n"
#define STOP		0xFFF0
int parent_pipe[2];
int child_pipe[2];
int fd;
pid_t parent_pid, child_pid;
void parent_put();
void parent_get();
void child_put();
void child_get();
void stop_child();
void compare_lock(struct flock *, short, short, int, int, pid_t);
void unlock_file();
void do_test(struct flock *, short, short, int, int);
void catch_child();
char *str_type();
int do_lock(int, short, short, int, int);
char *TCID = "fcntl11";
int TST_TOTAL = 1;
int fail;
void cleanup(void)
{
	tst_rmdir();
}
void setup(void)
{
	char *buf = STRING;
	char template[PATH_MAX];
	struct sigaction act;
	tst_sig(FORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	umask(0);
	TEST_PAUSE;
	pipe(cleanup, parent_pipe);
	pipe(cleanup, child_pipe);
	parent_pid = getpid();
	snprintf(template, PATH_MAX, "fcntl11XXXXXX");
	if ((fd = mkstemp(template)) < 0)
		tst_resm(TFAIL, "Couldn't open temp file! errno = %d", errno);
	write(cleanup, 
	memset(&act, 0, sizeof(act));
	act.sa_handler = catch_child;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGCHLD);
	if ((sigaction(SIGCHLD, &act, NULL)) < 0)
		tst_brkm(TBROK | TERRNO, cleanup,
			 "sigaction(SIGCHLD, ..) failed");
}
void do_child(void)
{
	struct flock fl;
	close(parent_pipe[1]);
	close(child_pipe[0]);
	while (1) {
		child_get(&fl);
		if (fcntl(fd, F_GETLK, &fl) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		child_put(&fl);
	}
}
int do_lock(int cmd, short type, short whence, int start, int len)
{
	struct flock fl;
	fl.l_type = type;
	fl.l_whence = whence;
	fl.l_start = start;
	fl.l_len = len;
	return (fcntl(fd, cmd, &fl));
}
void do_test(struct flock *fl, short type, short whence, int start, int len)
{
	fl->l_type = type;
	fl->l_whence = whence;
	fl->l_start = start;
	fl->l_len = len;
	fl->l_pid = (short)0;
	parent_put(fl);
	parent_get(fl);
}
void
compare_lock(struct flock *fl, short type, short whence, int start, int len,
	     pid_t pid)
{
	if (fl->l_type != type)
		tst_resm(TFAIL, "lock type is wrong should be %s is %s",
			 str_type(type), str_type(fl->l_type));
	if (fl->l_whence != whence)
		tst_resm(TFAIL, "lock whence is wrong should be %d is %d",
			 whence, fl->l_whence);
	if (fl->l_start != start)
		tst_resm(TFAIL, "region starts in wrong place, should be "
			 "%d is %" PRId64, start, (int64_t) fl->l_start);
	if (fl->l_len != len)
		tst_resm(TFAIL,
			 "region length is wrong, should be %d is %" PRId64,
			 len, (int64_t) fl->l_len);
	if (fl->l_pid != pid)
		tst_resm(TFAIL, "locking pid is wrong, should be %d is %d",
			 pid, fl->l_pid);
}
void unlock_file(void)
{
	struct flock fl;
	if (do_lock(F_SETLK, (short)F_UNLCK, (short)0, 0, 0) < 0)
		tst_resm(TFAIL | TERRNO, "fcntl on file failed");
	do_test(&fl, F_WRLCK, 0, 0, 0);
	compare_lock(&fl, (short)F_UNLCK, (short)0, 0, 0, (pid_t) 0);
}
char *str_type(int type)
{
	

char buf[20];
	switch (type) {
	case 0:
		return ("F_RDLCK");
	case 1:
		return ("F_WRLCK");
	case 2:
		return ("F_UNLCK");
	default:
		sprintf(buf, "BAD VALUE: %d", type);
		return (buf);
	}
}
void parent_put(struct flock *l)
{
	write(cleanup, 1, parent_pipe[1], l, sizeofcleanup, 1, parent_pipe[1], l, sizeof*l));
}
void parent_get(struct flock *l)
{
	read(cleanup, 1, child_pipe[0], l, sizeofcleanup, 1, child_pipe[0], l, sizeof*l));
}
void child_put(struct flock *l)
{
	write(NULL, 1, child_pipe[1], l, sizeofNULL, 1, child_pipe[1], l, sizeof*l));
}
void child_get(struct flock *l)
{
	read(NULL, 1, parent_pipe[0], l, sizeofNULL, 1, parent_pipe[0], l, sizeof*l));
	if (l->l_type == (short)STOP)
		exit(0);
}
void stop_child(void)
{
	struct flock fl;
	signal(SIGCHLD, SIG_DFL);
	fl.l_type = STOP;
	parent_put(&fl);
	wait(0);
}
void catch_child(void)
{
	tst_brkm(TFAIL, cleanup, "Unexpected death of child process");
}
int main(int ac, char **av)
{
	struct flock tl;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
#ifdef UCLINUX
	maybe_run_child(&do_child, "ddddd", &parent_pipe[0],
			&parent_pipe[1], &child_pipe[0], &child_pipe[1], &fd);
#endif
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
#ifdef UCLINUX
			if (self_exec(av[0], "ddddd", parent_pipe[0],
				      parent_pipe[1], child_pipe[0],
				      child_pipe[1], fd) < 0)
				tst_brkm(TBROK | TERRNO, cleanup,
					 "self_exec failed");
#else
			do_child();
#endif
		} else if (child_pid == -1)
			tst_brkm(TBROK | TERRNO, cleanup, "fork failed");
		close(cleanup, parent_pipe[0]);
		close(cleanup, child_pipe[1]);
		tst_resm(TINFO, "Enter block 1");
		 * Add a write lock to the middle of the file and a read
		 * at the begining
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 1, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Test read lock
		 */
		do_test(&tl, F_WRLCK, 0, 0, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 1, 5, parent_pid);
		 * Test write lock
		 */
		do_test(&tl, F_WRLCK, 0, 6, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 5, parent_pid);
		 * Test that the rest of the file is unlocked
		 */
		do_test(&tl, F_WRLCK, 0, 15, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 0, 0);
		 * remove all the locks set above
		 */
		unlock_file();
		tst_resm(TINFO, "Exit block 1");
		tst_resm(TINFO, "Enter block 2");
		 * Set a write lock at the middle of the file and a
		 * read lock just before
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 5, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Test the read lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 5, 5, parent_pid);
		 * Test the write lock.
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 10, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 5, parent_pid);
		 * Test to make sure the rest of the file is unlocked.
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 0, 0);
		 * remove all the locks set above
		 */
		unlock_file();
		tst_resm(TINFO, "Exit block 2");
		tst_resm(TINFO, "Enter block 3");
		 * Set a write lock in the middle and a read lock that
		 * ends at the first byte of the write lock
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 5, 6) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Test read lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 5, 6, parent_pid);
		 * Test write lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 11, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 11, 4, parent_pid);
		 * Test to make sure the rest of the file is unlocked.
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 0, 0);
		 * remove all the locks set above
		 */
		unlock_file();
		tst_resm(TINFO, "Exit block 3");
		tst_resm(TINFO, "Enter block 4");
		 * Set a write lock on the middle of the file and a read
		 * lock that overlaps the front of the write.
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 5, 8) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Test the read lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 5, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 5, 8, parent_pid);
		 * Test the write lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 13, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 13, 2, parent_pid);
		 * Test to make sure the rest of the file is unlocked.
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 0, 0);
		 * remove all the locks set above
		 */
		unlock_file();
		tst_resm(TINFO, "Exit block 4");
		tst_resm(TINFO, "Enter block 5");
		 * Set a write lock in the middle of a file and a read
		 * lock in the middle of it
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 10) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 13, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Test the first write lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 3, parent_pid);
		 * Test the read lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 13, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 13, 5, parent_pid);
		 * Test the second write lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 18, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 18, 2, parent_pid);
		 * Test to make sure the rest of the file is unlocked
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 20, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 20, 0, 0);
		 * remove all the locks set above.
		 */
		unlock_file();
		tst_resm(TINFO, "Exit block 5");
		tst_resm(TINFO, "Enter block 6");
		 * Set a write lock in the middle of the file and a read
		 * lock that overlaps the end
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Set a read lock on the whole file
		 */
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 13, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Test the write lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 3, parent_pid);
		 * Test the read lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 13, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 13, 5, parent_pid);
		 * Test to make sure the rest of the file is unlocked
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 18, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 18, 0, 0);
		 * remove all the locks set above
		 */
		unlock_file();
		tst_resm(TINFO, "Exit block 6");
		tst_resm(TINFO, "Enter block 7");
		 * Set a write lock in the middle of the file and a read
		 * lock starting at the last byte of the write lock
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Set a read lock on the whole file.
		 */
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 14, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Test write lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 4, parent_pid);
		 * Test the read lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 14, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 14, 5, parent_pid);
		 * Test to make sure the end of the file is unlocked
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 19, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 19, 0, 0);
		 * remove all the locks set above
		 */
		unlock_file();
		tst_resm(TINFO, "Exit block 7");
		tst_resm(TINFO, "Enter block 8");
		 * Set a write lock in the middle of the file and a read
		 * lock that starts just after the last byte of the
		 * write lock.
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Set a read lock on the whole file
		 */
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 15, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Test the write lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 5, parent_pid);
		 * Test the read lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 15, 5, parent_pid);
		 * Test to make sure the rest of the file is unlocked
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 20, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 20, 0, 0);
		 * remove all the locks set above
		 */
		unlock_file();
		tst_resm(TINFO, "Exit block 8");
		tst_resm(TINFO, "Enter block 9");
		 * Set a write lock at the middle of the file and a read
		 * lock that starts past the end of the write lock.
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 16, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed");
		 * Test the write lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 5, parent_pid);
		 * Test that byte in between is unlocked
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 1);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 1, 0);
		 * Test the read lock
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 16, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 16, 5, parent_pid);
		 * Test to make sure the rest of the file is unlocked
		 */
		do_test(&tl, (short)F_WRLCK, (short)0, 21, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 21, 0, 0);
		 * remove all the locks set above
		 */
		unlock_file();
		tst_resm(TINFO, "Exit block 9");
		stop_child();
		close(cleanup, fd);
	}
	cleanup();
	tst_exit();
}

