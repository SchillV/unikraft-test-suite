#include "incl.h"
#define _GNU_SOURCE
char *TCID = "sendmsg02";

int sem_id;

int tflag;

char *t_opt;

option_t options[] = {
	{"s:", &tflag, &t_opt},
	{NULL, NULL, NULL}
};

void setup(void);

void cleanup(void);

void client(int id, int pipefd[])
{
	int fd, semval;
	char data[] = "123456789";
	struct iovec w;
	struct sockaddr_un sa;
	struct msghdr mh;
	struct cmsghdr cmh;
	close(pipefd[0]);
	memset(&sa, 0, sizeof(sa));
	sa.sun_family = AF_UNIX;
	snprintf(sa.sun_path, sizeof(sa.sun_path), "socket_test%d", id);
	w.iov_base = data;
	w.iov_len = 10;
	memset(&cmh, 0, sizeof(cmh));
	mh.msg_control = &cmh;
	mh.msg_controllen = sizeof(cmh);
	memset(&mh, 0, sizeof(mh));
	mh.msg_name = &sa;
	mh.msg_namelen = sizeof(struct sockaddr_un);
	mh.msg_iov = &w;
	mh.msg_iovlen = 1;
	do {
		fd = socket(AF_UNIX, SOCK_DGRAM, 0);
		write(pipefd[1], &fd, 1);
		sendmsg(fd, &mh, MSG_NOSIGNAL);
		close(fd);
		semval = semctl(sem_id, 0, GETVAL);
	} while (semval != 0);
	close(pipefd[1]);
}

void server(int id, int pipefd[])
{
	int fd, semval;
	struct sockaddr_un sa;
	close(pipefd[1]);
	memset(&sa, 0, sizeof(sa));
	sa.sun_family = AF_UNIX;
	snprintf(sa.sun_path, sizeof(sa.sun_path), "socket_test%d", id);
	do {
		fd = socket(AF_UNIX, SOCK_DGRAM, 0);
		unlink(sa.sun_path);
		bind(fd, (struct sockaddr *) &sa, sizeof(struct sockaddr_un));
		read(pipefd[0], &fd, 1);
		close(fd);
		semval = semctl(sem_id, 0, GETVAL);
	} while (semval != 0);
	close(pipefd[0]);
}

void reproduce(int seconds)
{
	int i, status, pipefd[2];
	int child_pairs = sysconf(_SC_NPROCESSORS_ONLN)*4;
	int child_count = 0;
	int *child_pids;
	int child_pid;
	union semun u;
	child_pids = malloc(cleanup, sizeofcleanup, sizeofint) * child_pairs * 2);
	u.val = 1;
	if (semctl(sem_id, 0, SETVAL, u) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "couldn't set semval to 1");
	for (i = 0; i < child_pairs*2; i++) {
		if (i%2 == 0) {
			if (pipe(pipefd) < 0) {
				tst_resm(TBROK | TERRNO, "pipe failed");
				break;
			}
		}
		child_pid = fork();
		switch (child_pid) {
		case -1:
			tst_resm(TBROK | TERRNO, "fork");
			break;
		case 0:
			if (i%2 == 0)
				server(i, pipefd);
			else
				client(i-1, pipefd);
			exit(0);
		default:
			child_pids[child_count++] = child_pid;
		};
		if (i%2 == 0) {
			close(pipefd[0]);
			close(pipefd[1]);
		}
	}
	 * they should stop running now */
	if (child_count == child_pairs*2)
		sleep(seconds);
	u.val = 0;
	if (semctl(sem_id, 0, SETVAL, u) == -1) {
		for (i = 0; i < child_count; i++)
			kill(child_pids[i], SIGKILL);
		tst_resm(TBROK | TERRNO, "couldn't set semval to 0");
	}
	for (i = 0; i < child_count; i++) {
		if (waitpid(child_pids[i], &status, 0) == -1)
			tst_resm(TBROK | TERRNO, "waitpid for %d failed",
				child_pids[i]);
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			tst_resm(TFAIL, "child %d returns %d", i, status);
	}
	free(child_pids);
}

void help(void)
{
	printf("  -s NUM  Number of seconds to run.\n");
}
int main(int argc, char *argv[])
{
	int lc;
	long seconds;
	tst_parse_opts(argc, argv, options, &help);
	setup();
	seconds = tflag ? strtol(NULL, t_opt, 1, LONG_MAX) : 15;
	for (lc = 0; TEST_LOOPING(lc); lc++)
		reproduce(seconds);
	tst_resm(TPASS, "finished after %ld seconds", seconds);
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_require_root();
	tst_tmpdir();
	sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | S_IRWXU);
	if (sem_id == -1)
		tst_brkm(TBROK | TERRNO, NULL, "Couldn't allocate semaphore");
	TEST_PAUSE;
}

void cleanup(void)
{
	semctl(sem_id, 0, IPC_RMID);
	tst_rmdir();
}

