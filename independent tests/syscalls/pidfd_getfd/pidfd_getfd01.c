#include "incl.h"
#define TESTFILE "testfile"

int fds[2] = {-1, -1};

int pidfd = -1;

void do_child(void)
{
	int fd;
	close(fds[0]);
	fd = creat(TESTFILE, 0644);
	write(1, fds[1], &fd, sizeof1, fds[1], &fd, sizeoffd));
	TST_CHECKPOINT_WAIT(0);
	close(fd);
	close(fds[1]);
	exit(0);
}

void run(void)
{
	int flag, pid, targetfd, remotefd;
	pipe(fds);
	pid = fork();
	if (!pid)
		do_child();
	close(fds[1]);
	TST_PROCESS_STATE_WAIT(pid, 'S', 0);
	pidfd = pidfd_open(pid, 0);
	read(1, fds[0], &targetfd, sizeof1, fds[0], &targetfd, sizeoftargetfd));
	TST_EXP_FD_SILENT(pidfd_getfd(pidfd, targetfd, 0),
		"pidfd_getfd(%d, %d , 0)", pidfd, targetfd);
	remotefd = TST_RET;
	flag = fcntl(remotefd, F_GETFD);
	if (!(flag & FD_CLOEXEC))
		tst_res(TFAIL, "pidfd_getfd() didn't set close-on-exec flag");
	TST_EXP_VAL_SILENT(kcmp(getpid(), pid, KCMP_FILE, remotefd, targetfd), 0);
	tst_res(TPASS, "pidfd_getfd(%d, %d, 0) passed", pidfd, targetfd);
	TST_CHECKPOINT_WAKE(0);
	close(remotefd);
	close(pidfd);
	close(fds[0]);
	tst_reap_children();
}

void setup(void)
{
	pidfd_open_supported();
	pidfd_getfd_supported();
}

void cleanup(void)
{
	if (fds[0] > -1)
		close(fds[0]);
	if (fds[1] > -1)
		close(fds[1]);
	if (pidfd > -1)
		close(pidfd);
}

void main(){
	setup();
	cleanup();
}
