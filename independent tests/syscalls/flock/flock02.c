#include "incl.h"

int badfd = -1;

int fd;

struct tcase {
	int *fd;
	int operation;
	int exp_err;
} tcases[] = {
	{&badfd, LOCK_SH, EBADF},
	{&fd, LOCK_NB, EINVAL},
	{&fd, LOCK_SH | LOCK_EX, EINVAL},
};

int  verify_flock(unsigned n)
{
	struct tcase *tc = &tcases[n];
	fd = open("testfile", O_RDWR);
flock(*tc->fd, tc->operation);
	if (TST_RET == 0) {
		tst_res(TFAIL | TTERRNO, "flock() succeeded unexpectedly");
		close(fd);
		return;
	}
	if (tc->exp_err == TST_ERR) {
		tst_res(TPASS | TTERRNO, "flock() failed expectedly");
	} else {
		tst_res(TFAIL | TTERRNO, "flock() failed unexpectedly, "
			"expected %s", tst_strerrno(tc->exp_err));
	}
	close(fd);
}

void setup(void)
{
	int fd1;
	fd1 = open("testfile", O_CREAT | O_TRUNC | O_RDWR, 0666);
	close(fd1);
}

void main(){
	setup();
	cleanup();
}
