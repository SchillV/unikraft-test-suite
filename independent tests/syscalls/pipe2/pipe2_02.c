#include "incl.h"
#define _GNU_SOURCE
#define TESTBIN "pipe2_02_child"

int fds[2];

void cleanup(void)
{
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[1] > 0)
		close(fds[1]);
}

int  verify_pipe2(
{
	int pid, status;
	char buf[20];
	pipe2(fds, O_CLOEXEC);
	sprintf(buf, "%d", fds[1]);
	pid = fork();
	if (pid == 0)
		execlp(TESTBIN, TESTBIN, buf, NULL);
	wait(&status);
	if (WIFEXITED(status)) {
		switch (WEXITSTATUS(status)) {
		case 0:
			tst_res(TPASS, "test O_CLOEXEC for pipe2 success");
		break;
		case 1:
			tst_res(TFAIL, "test O_CLOEXEC for pipe2 failed");
		break;
		default:
			tst_brk(TBROK, "execlp() failed");
		}
	} else {
		tst_brk(TBROK, "%s exits with unexpected error", TESTBIN);
	}
	cleanup();
}

void main(){
	setup();
	cleanup();
}
