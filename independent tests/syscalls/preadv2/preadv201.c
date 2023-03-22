#include "incl.h"
#define _GNU_SOURCE
#define CHUNK           64

int fd;

char buf[CHUNK];

struct iovec rd_iovec[] = {
	{buf, CHUNK},
	{NULL, 0},
};

struct tcase {
	off_t seek_off;
	int count;
	off_t read_off;
	ssize_t size;
	char content;
	off_t exp_off;
} tcases[] = {
	{0,     1, 0,           CHUNK,     'a', 0},
	{CHUNK, 2, 0,           CHUNK,     'a', CHUNK},
	{0,     1, CHUNK*3 / 2, CHUNK / 2, 'b', 0},
	{0,     1, -1,          CHUNK,     'a', CHUNK},
	{0,     2, -1,          CHUNK,     'a', CHUNK},
	{CHUNK, 1, -1,          CHUNK,     'b', CHUNK * 2},
};

int  verify_preadv2(unsigned int n)
{
	int i;
	char *vec;
	struct tcase *tc = &tcases[n];
	vec = rd_iovec[0].iov_base;
	memset(vec, 0x00, CHUNK);
	lseek(fd, tc->seek_off, SEEK_SET);
preadv2(fd, rd_iovec, tc->count, tc->read_off, 0);
	if (TST_RET < 0) {
		tst_res(TFAIL | TTERRNO, "preadv2() failed");
		return;
	}
	if (TST_RET != tc->size) {
		tst_res(TFAIL, "preadv2() read %li bytes, expected %zi",
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
	if (lseek(fd, 0, SEEK_CUR) != tc->exp_off) {
		tst_res(TFAIL, "preadv2() has changed file offset");
		return;
	}
	tst_res(TPASS, "preadv2() read %zi bytes with content '%c' expectedly",
		tc->size, tc->content);
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
