#include "incl.h"

void setup(void);

void cleanup(void);

void help(void);
char *TCID = "fcntl07";

char *t_opt;
option_t options[] = {
	{"T:", NULL, &t_opt},
	{NULL, NULL, NULL}
};

int file_fd, pipe_fds[2], fifo_fd;
#define FIFONAME "fifo"

struct tcase {
	int *fd;
	const char *msg;
} tcases[] = {
	{&file_fd, "regular file"},
	{pipe_fds, "pipe (write end)"},
	{pipe_fds+1, "pipe (read end)"},
	{&fifo_fd, "fifo"},
};
int TST_TOTAL = ARRAY_SIZE(tcases);

int test_open(char *arg);

int  verify_cloexec(struct tcase *tc)
{
	int fd = *(tc->fd);
	char pidname[255];
	int status, pid;
fcntl(fd, F_SETFD, FD_CLOEXEC);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL | TTERRNO,
			 "fcntl(%s[%d], F_SETFD, FD_CLOEXEC) failed",
			 tc->msg, fd);
		return;
	}
	sprintf(pidname, "%d", fd);
	switch (pid = FORK_OR_VFORK()) {
	case -1:
		tst_resm(TBROK | TERRNO, "fork() failed");
		return;
	case 0:
		execlp(TCID, TCID, "-T", pidname, NULL);
		fprintf(stderr, "exec(%s, %s, -T, %s) failed.  Errno %s [%d]\n",
			TCID, TCID, pidname, strerror(errno), errno);
		exit(2);
	default:
	break;
	}
	waitpid(pid, &status, 0);
	if (!WIFEXITED(status)) {
		tst_resm(TBROK, "waitpid return was 0%o", status);
		return;
	}
	switch ((WEXITSTATUS(status))) {
	case 2:
		tst_resm(TBROK, "exec failed");
	break;
	case 0:
		tst_resm(TPASS, "%s CLOEXEC fd was closed after exec()",
			 tc->msg);
	break;
	default:
		tst_resm(TFAIL, "%s child exited non-zero, %d",
			 tc->msg, WEXITSTATUS(status));
	}
}
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, options, &help);
	if (t_opt)
		exit(test_open(t_opt));
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			verify_cloexec(tcases + i);
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	file_fd = open(cleanup, "test_file", O_CREAT | O_RDWR, 0666);
	pipe(cleanup, pipe_fds);
	mkfifo(cleanup, FIFONAME, 0666);
	fifo_fd = open(cleanup, FIFONAME, O_RDWR, 0666);
}
void cleanup(void)
{
	if (file_fd > 0 && close(file_fd))
		tst_resm(TWARN | TERRNO, "close(file_fd) failed");
	if (pipe_fds[0] > 0 && close(pipe_fds[0]))
		tst_resm(TWARN | TERRNO, "close(pipe_fds[0]) failed");
	if (pipe_fds[1] > 0 && close(pipe_fds[1]))
		tst_resm(TWARN | TERRNO, "close(pipe_fds[1]) failed");
	if (fifo_fd > 0 && close(fifo_fd))
		tst_resm(TWARN | TERRNO, "close(fifo_fd) failed");
	tst_rmdir();
}
void help(void)
{
	printf("  -T fd   The program runs as 'test_open()'\n");
}
int test_open(char *arg)
{
	int fd, rc;
	int status;
	fd = atoi(arg);
	rc = fcntl(fd, F_GETFD, &status);
	if (rc == -1 && errno == EBADF)
		return 0;
	fprintf(stderr, "fcntl() returned %i, errno %s(%i)\n",
		rc, tst_strerrno(errno), errno);
	return 1;
}

