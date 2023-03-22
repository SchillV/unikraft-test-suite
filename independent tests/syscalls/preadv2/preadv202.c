#include "incl.h"
#define _GNU_SOURCE
#define CHUNK           64

int fd1;

int fd2;

int fd3 = -1;

int fd4;

int fd5[2];

char buf[CHUNK];

struct iovec rd_iovec1[] = {
	{buf, -1},
};

struct iovec rd_iovec2[] = {
	{buf, CHUNK},
};

struct iovec rd_iovec3[] = {
	{NULL, CHUNK},
};

struct tcase {
	int *fd;
	struct iovec *name;
	int count;
	off_t offset;
	int flag;
	int exp_err;
} tcases[] = {
	{&fd1, rd_iovec1, 1, 0, 0, EINVAL},
	{&fd1, rd_iovec2, -1, 0, 0, EINVAL},
	{&fd1, rd_iovec2, 1, 1, -1, EOPNOTSUPP},
	{&fd1, rd_iovec3, 1, 0, 0, EFAULT},
	{&fd3, rd_iovec2, 1, 0, 0, EBADF},
	{&fd2, rd_iovec2, 1, 0, 0, EBADF},
	{&fd4, rd_iovec2, 1, 0, 0, EISDIR},
	{&fd5[0], rd_iovec2, 1, 0, 0, ESPIPE},
};

int  verify_preadv2(unsigned int n)
{
	struct tcase *tc = &tcases[n];
preadv2(*tc->fd, tc->name, tc->count, tc->offset, tc->flag);
	if (TST_RET == 0) {
		tst_res(TFAIL, "preadv2() succeeded unexpectedly");
		return;
	}
	if (TST_ERR == tc->exp_err) {
		tst_res(TPASS | TTERRNO, "preadv2() failed as expected");
		return;
	}
	tst_res(TFAIL | TTERRNO, "preadv2() failed unexpectedly, expected %s",
		tst_strerrno(tc->exp_err));
}

void setup(void)
{
	fd1 = open("file1", O_RDWR | O_CREAT, 0644);
	ftruncate(fd1, getpagesizefd1, getpagesize));
	fd2 = open("file2", O_WRONLY | O_CREAT, 0644);
	fd4 = open(".", O_RDONLY);
	pipe(fd5);
	rd_iovec3[0].iov_base = tst_get_bad_addr(NULL);
}

void cleanup(void)
{
	if (fd1 > 0)
		close(fd1);
	if (fd2 > 0)
		close(fd2);
	if (fd4 > 0)
		close(fd4);
	if (fd5[0] > 0)
		close(fd5[0]);
	if (fd5[1] > 0)
		close(fd5[1]);
}

void main(){
	setup();
	cleanup();
}
