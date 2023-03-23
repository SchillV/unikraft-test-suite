#include "incl.h"

void setup(void);

void cleanup(void);

void help(void);
char *TCID = "fcntl07";

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
int TST_TOTAL = sizeof(tcases) / sizeof(tcases[0]);

int test_open(char *arg);

int  verify_cloexec(struct tcase *tc)
{
	int fd = *(tc->fd);
	char pidname[255];
	int status, pid;
	int ret = fcntl(fd, F_SETFD, FD_CLOEXEC);
	if (ret == -1) {
		printf("fcntl(%s[%d], F_SETFD, FD_CLOEXEC) failed, error number %d\n",tc->msg, fd, errno);
		return 0;
	}
	sprintf(pidname, "%d", fd);
	switch (pid = fork()) {
	case -1:
		printf("fork() failed\n");
		return 0;
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
		printf("waitpid return was 0%o\n", status);
		return 0;
	}
	switch ((WEXITSTATUS(status))) {
	case 2:
		printf("exec failed\n");
		return 0;
	break;
	case 0:
		printf("%s CLOEXEC fd was closed after exec()\n",tc->msg);
		return 1;
	break;
	default:
		printf("%s child exited non-zero, %d\n", tc->msg, WEXITSTATUS(status));
		return 0;
	}
}
int main(int ac, char **av)
{
	int i, ok = 1;
	setup();
	for (i = 0; i < TST_TOTAL; i++)
		ok *= verify_cloexec(tcases + i);
	if(ok)
		printf("test succeeded\n");
	cleanup();
}
void setup(void)
{
	file_fd = open("test_file", O_CREAT | O_RDWR, 0666);
	pipe(pipe_fds);
	mkfifo(FIFONAME, 0666);
	fifo_fd = open(FIFONAME, O_RDWR, 0666);
}
void cleanup(void)
{
	if (file_fd > 0 && close(file_fd))
		printf("close(file_fd) failed\n");
	if (pipe_fds[0] > 0 && close(pipe_fds[0]))
		printf("close(pipe_fds[0]) failed, error number %d\n", errno);
	if (pipe_fds[1] > 0 && close(pipe_fds[1]))
		printf("close(pipe_fds[1]) failed, error number %d\n", errno);
	if (fifo_fd > 0 && close(fifo_fd))
		printf("close(fifo_fd) failed, error number %d\n", errno);
}

int test_open(char *arg)
{
	int fd, rc;
	int status;
	fd = atoi(arg);
	rc = fcntl(fd, F_GETFD, &status);
	if (rc == -1 && errno == EBADF)
		return 0;
	fprintf(stderr, "fcntl() returned %i, errno %d\n",
		rc, errno);
	return 1;
}

