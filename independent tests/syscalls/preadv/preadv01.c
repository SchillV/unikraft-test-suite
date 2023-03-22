#include "incl.h"
* Copyright (c) 2015 Fujitsu Ltd.
* Author: Xiao Yang <yangx.jy@cn.fujitsu.com>
*/
* Test Name: preadv01
*
* Test Description:
* Testcase to check the basic functionality of the preadv(2).
* Preadv(2) should succeed to read the expected content of data
* and after reading the file, the file offset is not changed.
*/
#define _GNU_SOURCE
#define CHUNK           64

int fd;

struct iovec *rd_iovec;

struct tcase {
	int count;
	off_t offset;
	ssize_t size;
	char content;
} tcases[] = {
	{1, 0, CHUNK, 'a'},
	{2, 0, CHUNK, 'a'},
	{1, CHUNK*3/2, CHUNK/2, 'b'}
};
int  verify_preadv(unsigned int n)
{
	int i;
	char *vec;
	struct tcase *tc = &tcases[n];
	vec = rd_iovec[0].iov_base;
	memset(vec, 0x00, CHUNK);
	lseek(fd, 0, SEEK_SET);
preadv(fd, rd_iovec, tc->count, tc->offset);
	if (TST_RET < 0) {
		tst_res(TFAIL | TTERRNO, "Preadv(2) failed");
		return;
	}
	if (TST_RET != tc->size) {
		tst_res(TFAIL, "Preadv(2) read %li bytes, expected %zi",
			 TST_RET, tc->size);
		return;
	}
	for (i = 0; i < tc->size; i++) {
		if (vec[i] != tc->content)
			break;
	}
	if (i < tc->size) {
		tst_res(TFAIL, "Buffer wrong at %i have %02x expected %02x",
			 i, vec[i], tc->content);
		return;
	}
	if (lseek(fd, 0, SEEK_CUR) != 0) {
		tst_res(TFAIL, "Preadv(2) has changed file offset");
		return;
	}
	tst_res(TPASS, "Preadv(2) read %zi bytes successfully "
		 "with content '%c' expectedly", tc->size, tc->content);
}
void setup(void)
{
	char buf[CHUNK];
	fd = open("file", O_RDWR | O_CREAT, 0644);
	memset(buf, 'a', sizeof(buf));
	write(1, fd, buf, sizeof1, fd, buf, sizeofbuf));
	memset(buf, 'b', sizeof(buf));
	write(1, fd, buf, sizeof1, fd, buf, sizeofbuf));
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
