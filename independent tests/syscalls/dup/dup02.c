#include "incl.h"

struct tcase {
	int fd;
	int expected_errno;
} tcases[] = {
	{-1, EBADF},
	{1500, EBADF},
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
dup(tc->fd);
	if (TST_RET < -1) {
		tst_res(TFAIL | TTERRNO, "Invalid dup() return value %ld",
			TST_RET);
		return;
	}
	if (TST_RET == -1) {
		if (tc->expected_errno == TST_ERR) {
			tst_res(TPASS | TTERRNO, "dup(%d) failed as expected",
				tc->fd);
		} else {
			tst_res(TFAIL | TTERRNO, "dup(%d) failed unexpectedly",
				tc->fd);
		}
		return;
	}
	tst_res(TFAIL, "dup(%d) succeeded unexpectedly", tc->fd);
	close(TST_RET);
}

void main(){
	setup();
	cleanup();
}
