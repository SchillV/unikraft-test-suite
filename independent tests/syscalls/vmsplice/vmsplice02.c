#include "incl.h"
#define _GNU_SOURCE
#define TESTFILE "testfile"
#define TEST_BLOCK_SIZE 128

char buffer[TEST_BLOCK_SIZE];

int notvalidfd = -1;

int filefd;

int pipes[2];

struct iovec ivc;

struct tcase {
	int *fd;
	const struct iovec *iov;
	unsigned long nr_segs;
	int exp_errno;
} tcases[] = {
	{ &notvalidfd, &ivc, 1, EBADF },
	{ &filefd, &ivc, 1, EBADF },
	{ &pipes[1], &ivc, IOV_MAX + 1, EINVAL },
};

void setup(void)
{
	filefd = open(TESTFILE, O_WRONLY | O_CREAT, 0644);
	pipe(pipes);
	ivc.iov_base = buffer;
	ivc.iov_len = TEST_BLOCK_SIZE;
}

int  vmsplice_verify(unsigned int n)
{
	struct tcase *tc = &tcases[n];
vmsplice(*(tc->fd), tc->iov, tc->nr_segs, 0);
	if (TST_RET != -1) {
		tst_res(TFAIL, "vmsplice() returned %ld, "
			"expected -1, errno:%d", TST_RET,
			tc->exp_errno);
		return;
	}
	if (TST_ERR != tc->exp_errno) {
		tst_res(TFAIL | TTERRNO,
			"vmsplice() failed unexpectedly; expected: %d - %s",
			tc->exp_errno, tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "vmsplice() failed as expected");
}

void cleanup(void)
{
	if (filefd > 0)
		close(filefd);
	if (pipes[0] > 0)
		close(pipes[0]);
	if (pipes[1] > 0)
		close(pipes[1]);
}

void main(){
	setup();
	cleanup();
}
