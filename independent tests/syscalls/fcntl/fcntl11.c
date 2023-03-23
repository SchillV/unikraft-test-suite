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
}
void setup(void)
{
	char *buf = STRING;
	char template[PATH_MAX];
	struct sigaction act;;
	umask(0);
	pipe(parent_pipe);
	pipe(child_pipe);
	parent_pid = getpid();
	snprintf(template, PATH_MAX, "fcntl11XXXXXX\n");
	if ((fd = mkstemp(template)) < 0)
		printf( "Couldn't open temp file! errno = %d", errno);
	write(fd, buf, STRINGSIZE);
	memset(&act, 0, sizeof(act));
	act.sa_handler = catch_child;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGCHLD);
	if ((sigaction(SIGCHLD, &act, NULL)) < 0)
		tst_brkm(TBROK | TERRNO, cleanup,
			 "sigaction(SIGCHLD, ..) failed\n");
}
void do_child(void)
{
	struct flock fl;
	close(parent_pipe[1]);
	close(child_pipe[0]);
	while (1) {
		child_get(&fl);
		if (fcntl(fd, F_GETLK, &fl) < 0){
			printf("fcntl on file failed, error number %d", errno);
			exit(0);
		}
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
	if (fl->l_type != type){
		printf("lock type is wrong should be %s is %s\n",
			 str_type(type), str_type(fl->l_type));
		exit(0);
	}if (fl->l_whence != whence){
		printf( "lock whence is wrong should be %d is %d\n",
			 whence, fl->l_whence);
		exit(0);
	}if (fl->l_start != start){
		printf( "region starts in wrong place, should be%d is % \n" PRId64, start, (int64_t) fl->l_start);
		exit(0);
	}if (fl->l_len != len){
		printf(
			 "region length is wrong, should be %d is % \n" PRId64,
			 len, (int64_t) fl->l_len);
		exit(0);
	}if (fl->l_pid != pid){
		printf( "locking pid is wrong, should be %d is %d\n",
			 pid, fl->l_pid);
		exit(0);
	}
}
void unlock_file(void)
{
	struct flock fl;
	if (do_lock(F_SETLK, (short)F_UNLCK, (short)0, 0, 0) < 0){
		printf("fcntl on file failed, error number %d\n", errno);
		exit(0);
	} do_test(&fl, F_WRLCK, 0, 0, 0);
	compare_lock(&fl, (short)F_UNLCK, (short)0, 0, 0, (pid_t) 0);
}
char *str_type(int type)
{
	char buf[20];
	switch (type) {
	case 0:
		return ("F_RDLCK\n");
	case 1:
		return ("F_WRLCK\n");
	case 2:
		return ("F_UNLCK\n");
	default:
		sprintf(buf, "BAD VALUE: %d", type);
		return (buf);
	}
}
void parent_put(struct flock *l)
{
	write(parent_pipe[1], l, sizeof(*l));
}
void parent_get(struct flock *l)
{
	read(child_pipe[0], l, sizeof(*l));
}
void child_put(struct flock *l)
{
	write(child_pipe[1], l, sizeof(*l));
}
void child_get(struct flock *l)
{
	read(parent_pipe[0], l, sizeof(*l));
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
	printf("Unexpected death of child process\n\n");
	cleanup();
	exit(0);
}
int main(int ac, char **av)
{
	struct flock tl;
	if ((child_pid = fork()) == 0) {
#ifdef UCLINUX
	maybe_run_child(&do_child, "ddddd", &parent_pipe[0],
			&parent_pipe[1], &child_pipe[0], &child_pipe[1], &fd);
#endif
#ifdef UCLINUX
			if (self_exec(av[0], "ddddd", parent_pipe[0],
				      parent_pipe[1], child_pipe[0],
				      child_pipe[1], fd) < 0)
				tst_brkm(TBROK | TERRNO, cleanup,
					 "self_exec failed\n");
#else
			do_child();
#endif
		} else if (child_pid == -1)
			tst_brkm(TBROK | TERRNO,  "fork failed\n");
		close( parent_pipe[0]);
		close( child_pipe[1]);
		printf("[I] Enter block 1\n");
		 * Add a write lock to the middle of the file and a read
		 * at the begining
		 */
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 1, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		do_test(&tl, F_WRLCK, 0, 0, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 1, 5, parent_pid);
		do_test(&tl, F_WRLCK, 0, 6, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 5, parent_pid);
		do_test(&tl, F_WRLCK, 0, 15, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 0, 0);
		unlock_file();
		printf("[I] Exit block 1\n");
		printf("[I] Enter block 2\n");
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 5, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 5, 5, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 10, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 5, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 0, 0);
		unlock_file();
		printf("[I] Exit block 2\n");
		printf("[I] Enter block 3\n");
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 5, 6) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 5, 6, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 11, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 11, 4, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 0, 0);
		unlock_file();
		printf("[I] Exit block 3\n");
		printf("[I] Enter block 4\n");
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 5, 8) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		do_test(&tl, (short)F_WRLCK, (short)0, 5, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 5, 8, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 13, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 13, 2, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 0, 0);
		unlock_file();
		printf("[I] Exit block 4\n");
		printf("[I] Enter block 5\n");
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 10) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 13, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 3, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 13, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 13, 5, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 18, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 18, 2, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 20, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 20, 0, 0);
		unlock_file();
		printf("[I] Exit block 5\n");
		printf("[I] Enter block 6\n");
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 13, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 3, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 13, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 13, 5, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 18, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 18, 0, 0);
		unlock_file();
		printf("[I] Exit block 6\n");
		printf("[I] Enter block 7\n");
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 14, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 4, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 14, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 14, 5, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 19, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 19, 0, 0);
		unlock_file();
		printf("[I] Exit block 7\n");
		printf("[I] Enter block 8\n");
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 15, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 5, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 15, 5, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 20, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 20, 0, 0);
		unlock_file();
		printf("[I] Exit block 8\n");
		printf("[I] Enter block 9\n");
		if (do_lock(F_SETLK, (short)F_WRLCK, (short)0, 10, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		if (do_lock(F_SETLK, (short)F_RDLCK, (short)0, 16, 5) < 0)
			tst_resm(TFAIL | TERRNO, "fcntl on file failed\n");
		do_test(&tl, (short)F_WRLCK, (short)0, 0, 0);
		compare_lock(&tl, (short)F_WRLCK, (short)0, 10, 5, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 15, 1);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 15, 1, 0);
		do_test(&tl, (short)F_WRLCK, (short)0, 16, 0);
		compare_lock(&tl, (short)F_RDLCK, (short)0, 16, 5, parent_pid);
		do_test(&tl, (short)F_WRLCK, (short)0, 21, 0);
		compare_lock(&tl, (short)F_UNLCK, (short)0, 21, 0, 0);
		unlock_file();
		printf("[I] Exit block 9\n");
		stop_child();
		close(fd);
	cleanup();
}

