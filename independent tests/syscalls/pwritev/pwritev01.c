#include "incl.h"
* Copyright (c) 2015 Fujitsu Ltd.
* Author: Xiao Yang <yangx.jy@cn.fujitsu.com>
*/
* Test Name: pwritev01
*
* Test Description:
* Testcase to check the basic functionality of the pwritev(2).
* pwritev(2) should succeed to write the expected content of data
* and after writing the file, the file offset is not changed.
*/
#define _GNU_SOURCE
#define	CHUNK		64

char buf[CHUNK];

char initbuf[CHUNK * 2];

char preadbuf[CHUNK];

int fd;

struct iovec wr_iovec[] = {
	{buf, CHUNK},
	{NULL, 0},
};

struct tcase {
	int count;
	off_t offset;
	ssize_t size;
} tcases[] = {
	{1, 0, CHUNK},
	{2, 0, CHUNK},
	{1, CHUNK/2, CHUNK},
};

int  verify_pwritev(unsigned int n)
{
	int i;
	struct tcase *tc = &tcases[n];
	pwrite(1, fd, initbuf, sizeof1, fd, initbuf, sizeofinitbuf), 0);
	lseek(fd, 0, SEEK_SET);
pwritev(fd, wr_iovec, tc->count, tc->offset);
	if (TST_RET < 0) {
		tst_res(TFAIL | TTERRNO, "pwritev() failed");
		return;
	}
	if (TST_RET != tc->size) {
		tst_res(TFAIL, "pwritev() wrote %li bytes, expected %zi",
			 TST_RET, tc->size);
		return;
	}
	if (lseek(fd, 0, SEEK_CUR) != 0) {
		tst_res(TFAIL, "pwritev() had changed file offset");
		return;
	}
	pread(1, fd, preadbuf, tc->size, tc->offset);
	for (i = 0; i < tc->size; i++) {
		if (preadbuf[i] != 0x61)
			break;
	}
	if (i != tc->size) {
		tst_res(TFAIL, "buffer wrong at %i have %02x expected 61",
			 i, preadbuf[i]);
		return;
	}
	tst_res(TPASS, "writev() wrote %zi bytes successfully "
		 "with content 'a' expectedly ", tc->size);
}

void setup(void)
{
	memset(&buf, 0x61, CHUNK);
	fd = open("file", O_RDWR | O_CREAT, 0644);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
