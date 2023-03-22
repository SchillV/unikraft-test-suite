#include "incl.h"
#define _GNU_SOURCE

char buffer[TEST_BLOCK_SIZE];

struct iovec *iov;

void vmsplice_test(void)
{
	int written, i;
	int pipes[2];
	char *arr_write = iov->iov_base;
	memset(iov->iov_base, 0, iov->iov_len);
	pipe(pipes);
	write(1, pipes[1], buffer, TEST_BLOCK_SIZE);
	written = vmsplice(pipes[0], iov, 1, 0);
	if (written < 0)
		tst_brk(TBROK | TERRNO, "vmsplice() failed");
	if (written == 0) {
		tst_res(TFAIL, "vmsplice() didn't write anything");
	} else {
		for (i = 0; i < TEST_BLOCK_SIZE; i++) {
			if (arr_write[i] != buffer[i]) {
				tst_res(TFAIL,
					"Wrong data in user memory at %i", i);
				break;
			}
		}
		if (i == written)
			tst_res(TPASS, "Spliced correctly into user memory");
	}
	close(pipes[1]);
	close(pipes[0]);
}

void setup(void)
{
	int i;
	for (i = 0; i < TEST_BLOCK_SIZE; i++)
		buffer[i] = i & 0xff;
}

void main(){
	setup();
	cleanup();
}
