#include "incl.h"
* Copyright (c) 2015-2016 Fujitsu Ltd.
* Author: Xiao Yang <yangx.jy@cn.fujitsu.com>
*/
* Test Name: pwritev02
*
* Description:
* 1) pwritev(2) fails if iov_len is invalid.
* 2) pwritev(2) fails if the vector count iovcnt is less than zero.
* 3) pwritev(2) fails if offset is negative.
* 4) pwritev(2) fails when attempts to write from a invalid address
* 5) pwritev(2) fails if file descriptor is invalid.
* 6) pwritev(2) fails if file descriptor is not open for writing.
* 7) pwritev(2) fails if fd is associated with a pipe.
*
* Expected Result:
* 1) pwritev(2) should return -1 and set errno to EINVAL.
* 2) pwritev(2) should return -1 and set errno to EINVAL.
* 3) pwritev(2) should return -1 and set errno to EINVAL.
* 4) pwritev(2) should return -1 and set errno to EFAULT.
* 5) pwritev(2) should return -1 and set errno to EBADF.
* 6) pwritev(2) should return -1 and set errno to EBADF.
* 7) pwritev(2) should return -1 and set errno to ESPIPE.
*/
#define _GNU_SOURCE
#define CHUNK           64

int fd1;

int fd2;

int fd3 = -1;

int fd4[2];

char buf[CHUNK];

struct iovec wr_iovec1[] = {
	{buf, -1},
};

struct iovec wr_iovec2[] = {
	{buf, CHUNK},
};

struct iovec wr_iovec3[] = {
	{(char *)-1, CHUNK},
};

struct tcase {
	int *fd;
	struct iovec *name;
	int count;
	off_t offset;
	int exp_err;
} tcases[] = {
	{&fd1, wr_iovec1, 1, 0, EINVAL},
	{&fd1, wr_iovec2, -1, 0, EINVAL},
	{&fd1, wr_iovec2, 1, -1, EINVAL},
	{&fd1, wr_iovec3, 1, 0, EFAULT},
	{&fd3, wr_iovec2, 1, 0, EBADF},
	{&fd2, wr_iovec2, 1, 0, EBADF},
	{&fd4[1], wr_iovec2, 1, 0, ESPIPE}
};

int  verify_pwritev(unsigned int n)
{
	struct tcase *tc = &tcases[n];
pwritev(*tc->fd, tc->name, tc->count, tc->offset);
	if (TST_RET == 0) {
		tst_res(TFAIL, "pwritev() succeeded unexpectedly");
		return;
	}
	if (TST_ERR == tc->exp_err) {
		tst_res(TPASS | TTERRNO, "pwritev() failed as expected");
		return;
	}
	tst_res(TFAIL | TTERRNO, "pwritev() failed unexpectedly, expected %s",
		tst_strerrno(tc->exp_err));
}

void setup(void)
{
	fd1 = open("file", O_RDWR | O_CREAT, 0644);
	ftruncate(fd1, getpagesizefd1, getpagesize));
	fd2 = open("file", O_RDONLY | O_CREAT, 0644);
	pipe(fd4);
}

void cleanup(void)
{
	if (fd1 > 0)
		close(fd1);
	if (fd2 > 0)
		close(fd2);
	if (fd4[0] > 0)
		close(fd4[0]);
	if (fd4[1] > 0)
		close(fd4[1]);
}

void main(){
	setup();
	cleanup();
}
