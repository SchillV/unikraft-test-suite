#include "incl.h"
#define _GNU_SOURCE

int pipes[2];

ssize_t pipe_max_size;

char *write_buffer;

struct iovec iov;

void vmsplice_test(void)
{
	int status;
	int pid;
vmsplice(pipes[1], &iov, 1, SPLICE_F_NONBLOCK);
	if (TST_RET < 0 && TST_ERR == EAGAIN) {
		tst_res(TPASS | TTERRNO,
		    "vmsplice(..., SPLICE_F_NONBLOCK) failed as expected");
	} else if (TST_RET < 0) {
		tst_res(TFAIL | TTERRNO,
		    "vmsplice(..., SPLICE_F_NONBLOCK) shall fail with EAGAIN");
	} else {
		tst_res(TFAIL,
		    "vmsplice(..., SPLICE_F_NONBLOCK) wrote to a full pipe");
	}
	pid = fork();
	if (!pid) {
vmsplice(pipes[1], &iov, 1, 0);
		if (TST_RET < 0)
			tst_res(TFAIL | TTERRNO, "vmsplice(..., 0) failed");
		else
			tst_res(TFAIL,
			    "vmsplice(..., 0) wrote to a full pipe");
		exit(0);
	}
	if (TST_PROCESS_STATE_WAIT(pid, 'S', 1000) < 0)
		return;
	else
		tst_res(TPASS, "vmsplice(..., 0) blocked");
	kill(pid, SIGKILL);
	wait(&status);
}

void cleanup(void)
{
	if (pipes[1] > 0)
		close(pipes[1]);
	if (pipes[0] > 0)
		close(pipes[0]);
}

void setup(void)
{
	pipe(pipes);
	pipe_max_size = fcntl(pipes[1], F_GETPIPE_SZ);
	write_buffer = tst_alloc(pipe_max_size);
	iov.iov_base = write_buffer;
	iov.iov_len = pipe_max_size;
vmsplice(pipes[1], &iov, 1, 0);
	if (TST_RET < 0) {
		tst_brk(TBROK | TTERRNO,
		    "Initial vmsplice() to fill pipe failed");
	}
}

void main(){
	setup();
	cleanup();
}
