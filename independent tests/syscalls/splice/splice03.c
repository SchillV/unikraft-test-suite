#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "testfile"
#define TEST_FILE2 "testfile2"
#define TEST_FILE3 "testfile3"
#define STR "abcdefghigklmnopqrstuvwxyz"
#define SPLICE_TEST_LEN 10

int badfd = -1;

int rdfd;

int wrfd;

int appendfd;

int pipes[2];

loff_t offset;

struct tcase {
	int *fdin;
	loff_t *offin;
	int *fdout;
	loff_t *offout;
	int exp_errno;
} tcases[] = {
	{ &badfd, NULL, &pipes[1], NULL, EBADF },
	{ &pipes[0], NULL, &badfd, NULL, EBADF },
	{ &wrfd, NULL, &pipes[1], NULL, EBADF },
	{ &pipes[0], NULL, &appendfd, NULL, EINVAL },
	{ &rdfd, NULL, &wrfd, NULL, EINVAL },
	{ &pipes[0], &offset, &wrfd, NULL, ESPIPE },
	{ &rdfd, NULL, &pipes[1], &offset, ESPIPE },
};

void setup(void)
{
	fprintf(TEST_FILE, STR);
	rdfd = open(TEST_FILE, O_RDONLY);
	wrfd = open(TEST_FILE2, O_WRONLY | O_CREAT, 0644);
	appendfd = open(TEST_FILE3, O_RDWR | O_CREAT | O_APPEND, 0644);
	pipe(pipes);
	write(1, pipes[1], STR, sizeof1, pipes[1], STR, sizeofSTR) - 1);
}

int  splice_verify(unsigned int n)
{
	struct tcase *tc = &tcases[n];
splice(*(tc->fdin), tc->offin, *(tc->fdout;
		tc->offout, SPLICE_TEST_LEN, 0));
	if (TST_RET != -1) {
		tst_res(TFAIL, "splice() returned %ld expected %s",
			TST_RET, tst_strerrno(tc->exp_errno));
		return;
	}
	if (TST_ERR != tc->exp_errno) {
		tst_res(TFAIL | TTERRNO,
			"splice() failed unexpectedly; expected: %d - %s",
			tc->exp_errno, tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "splice() failed as expected");
}

void cleanup(void)
{
	if (rdfd > 0)
		close(rdfd);
	if (wrfd > 0)
		close(wrfd);
	if (appendfd > 0)
		close(appendfd);
	if (pipes[0] > 0)
		close(pipes[0]);
	if (pipes[1] > 0)
		close(pipes[1]);
}

void main(){
	setup();
	cleanup();
}
