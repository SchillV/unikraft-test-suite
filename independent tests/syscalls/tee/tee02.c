#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "testfile"
#define STR "abcdefghigklmnopqrstuvwxyz"
#define TEE_TEST_LEN 10

int fd;

int pipes[2];

struct tcase {
	int *fdin;
	int *fdout;
	int exp_errno;
} tcases[] = {
	{ &fd, &pipes[1], EINVAL },
	{ &pipes[0], &fd, EINVAL },
	{ &pipes[0], &pipes[1], EINVAL },
};

void setup(void)
{
	fd = open(TEST_FILE, O_RDWR | O_CREAT, 0644);
	pipe(pipes);
	write(1, pipes[1], STR, sizeof1, pipes[1], STR, sizeofSTR) - 1);
}

int  tee_verify(unsigned int n)
{
	struct tcase *tc = &tcases[n];
tee(*(tc->fdin), *(tc->fdout), TEE_TEST_LEN, 0);
	if (TST_RET != -1) {
		tst_res(TFAIL, "tee() returned %ld, "
			"expected -1, errno:%d", TST_RET,
			tc->exp_errno);
		return;
	}
	if (TST_ERR != tc->exp_errno) {
		tst_res(TFAIL | TTERRNO,
			"tee() failed unexpectedly; expected: %d - %s",
			tc->exp_errno, tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "tee() failed as expected");
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	if (pipes[0] > 0)
		close(pipes[0]);
	if (pipes[1] > 0)
		close(pipes[1]);
}

void main(){
	setup();
	cleanup();
}
