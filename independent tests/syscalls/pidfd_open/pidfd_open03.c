#include "incl.h"

void run(void)
{
	struct pollfd pollfd;
	int fd, ready;
	pid_t pid;
	pid = fork();
	if (!pid) {
		TST_CHECKPOINT_WAIT(0);
		exit(EXIT_SUCCESS);
	}
	TST_EXP_FD_SILENT(pidfd_open(pid, 0), "pidfd_open(%d, 0)", pid);
	fd = TST_RET;
	TST_CHECKPOINT_WAKE(0);
	pollfd.fd = fd;
	pollfd.events = POLLIN;
	ready = poll(&pollfd, 1, -1);
	close(fd);
	waitpid(pid, NULL, 0);
	if (ready != 1)
		tst_res(TFAIL, "poll() returned %d", ready);
	else
		tst_res(TPASS, "pidfd_open() passed");
}

void main(){
	setup();
	cleanup();
}
