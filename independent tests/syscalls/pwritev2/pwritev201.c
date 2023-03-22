#include "incl.h"
#define _GNU_SOURCE
#define CHUNK	64

int fd;

char initbuf[CHUNK * 2];

char buf[CHUNK];

struct iovec wr_iovec[] = {
	{buf, CHUNK},
	{NULL, 0},
};

struct tcase {
	off_t seek_off;
	int count;
	off_t write_off;
	ssize_t size;
	off_t exp_off;
} tcases[] = {
	{0,     1, 0,          CHUNK, 0},
	{CHUNK, 2, 0,          CHUNK, CHUNK},
	{0,     1, CHUNK / 2,  CHUNK, 0},
	{0,     1, -1,         CHUNK, CHUNK},
	{0,     2, -1,         CHUNK, CHUNK},
	{CHUNK, 1, -1,         CHUNK, CHUNK * 2},
};

int  verify_pwritev2(unsigned int n)
{
	int i;
	char preadbuf[CHUNK];
	struct tcase *tc = &tcases[n];
	pwrite(1, fd, initbuf, sizeof1, fd, initbuf, sizeofinitbuf), 0);
	lseek(fd, tc->seek_off, SEEK_SET);
pwritev2(fd, wr_iovec, tc->count, tc->write_off, 0);
	if (TST_RET < 0) {
		tst_res(TFAIL | TTERRNO, "pwritev2() failed");
		return;
	}
	if (TST_RET != tc->size) {
		tst_res(TFAIL, "pwritev2() wrote %li bytes, expected %zi",
			 TST_RET, tc->size);
		return;
	}
	if (lseek(fd, 0, SEEK_CUR) != tc->exp_off) {
		tst_res(TFAIL, "pwritev2() had changed file offset");
		return;
	}
	memset(preadbuf, 0, CHUNK);
	if (tc->write_off != -1)
		pread(1, fd, preadbuf, tc->size, tc->write_off);
	else
		pread(1, fd, preadbuf, tc->size, tc->seek_off);
	for (i = 0; i < tc->size; i++) {
		if (preadbuf[i] != 0x61)
			break;
	}
	if (i != tc->size) {
		tst_res(TFAIL, "buffer wrong at %i have %c expected 'a'",
			 i, preadbuf[i]);
		return;
	}
	tst_res(TPASS, "pwritev2() wrote %zi bytes successfully "
		 "with content 'a' expectedly ", tc->size);
}

void setup(void)
{
	memset(buf, 0x61, CHUNK);
	memset(initbuf, 0, CHUNK * 2);
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
