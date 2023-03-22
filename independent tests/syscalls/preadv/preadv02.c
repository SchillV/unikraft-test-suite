#include "incl.h"
* Copyright (c) 2015-2016 Fujitsu Ltd.
* Author: Xiao Yang <yangx.jy@cn.fujitsu.com>
*/
* Test Name: preadv02
*
* Description:
* 1) preadv(2) fails if iov_len is invalid.
* 2) preadv(2) fails if the vector count iovcnt is less than zero.
* 3) preadv(2) fails if offset is negative.
* 4) preadv(2) fails when attempts to read into a invalid address.
* 5) preadv(2) fails if file descriptor is invalid.
* 6) preadv(2) fails if file descriptor is not open for reading.
* 7) preadv(2) fails when fd refers to a directory.
* 8) preadv(2) fails if fd is associated with a pipe.
*
* Expected Result:
* 1) preadv(2) should return -1 and set errno to EINVAL.
* 2) preadv(2) should return -1 and set errno to EINVAL.
* 3) preadv(2) should return -1 and set errno to EINVAL.
* 4) preadv(2) should return -1 and set errno to EFAULT.
* 5) preadv(2) should return -1 and set errno to EBADF.
* 6) preadv(2) should return -1 and set errno to EBADF.
* 7) preadv(2) should return -1 and set errno to EISDIR.
* 8) preadv(2) should return -1 and set errno to ESPIPE.
*/
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
	{(char *)-1, CHUNK},
};

struct tcase {
	int *fd;
	struct iovec *name;
	int count;
	off_t offset;
	int exp_err;
} tcases[] = {
	{&fd1, rd_iovec1, 1, 0, EINVAL},
	{&fd1, rd_iovec2, -1, 0, EINVAL},
	{&fd1, rd_iovec2, 1, -1, EINVAL},
	{&fd1, rd_iovec3, 1, 0, EFAULT},
	{&fd3, rd_iovec2, 1, 0, EBADF},
	{&fd2, rd_iovec2, 1, 0, EBADF},
	{&fd4, rd_iovec2, 1, 0, EISDIR},
	{&fd5[0], rd_iovec2, 1, 0, ESPIPE}
};

int  verify_preadv(unsigned int n)
{
	struct tcase *tc = &tcases[n];
preadv(*tc->fd, tc->name, tc->count, tc->offset);
	if (TST_RET == 0) {
		tst_res(TFAIL, "preadv() succeeded unexpectedly");
		return;
	}
	if (TST_ERR == tc->exp_err) {
		tst_res(TPASS | TTERRNO, "preadv() failed as expected");
		return;
	}
	tst_res(TFAIL | TTERRNO, "preadv() failed unexpectedly, expected %s",
		tst_strerrno(tc->exp_err));
}

void setup(void)
{
	fd1 = open("file1", O_RDWR | O_CREAT, 0644);
	ftruncate(fd1, getpagesizefd1, getpagesize));
	fd2 = open("file2", O_WRONLY | O_CREAT, 0644);
	fd4 = open(".", O_RDONLY);
	pipe(fd5);
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
