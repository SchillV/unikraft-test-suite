#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT	"mntpoint"
#define FNAME	MNTPOINT"/file"

char *initbuf, *preadbuf;

int fd;

off_t blk_off, zero_off;

ssize_t blksz;

struct iovec wr_iovec[] = {
	{NULL, 0},
	{NULL, 0},
};

struct tcase {
	int count;
	off_t *offset;
	ssize_t *size;
} tcases[] = {
	{1, &zero_off, &blksz},
	{2, &zero_off, &blksz},
	{1, &blk_off, &blksz},
};

int  verify_direct_pwritev(unsigned int n)
{
	int i;
	struct tcase *tc = &tcases[n];
	pwrite(1, fd, initbuf, blksz * 2, 0);
pwritev(fd, wr_iovec, tc->count, *tc->offset);
	if (TST_RET < 0) {
		tst_res(TFAIL | TTERRNO, "pwritev(O_DIRECT) fails");
		return;
	}
	if (TST_RET != *tc->size) {
		tst_res(TFAIL, "pwritev(O_DIRECT) wrote %li bytes, expected %zi",
			 TST_RET, *tc->size);
		return;
	}
	if (lseek(fd, 0, SEEK_CUR) != 0) {
		tst_res(TFAIL, "pwritev(O_DIRECT) had changed file offset");
		return;
	}
	memset(preadbuf, 0x00, blksz);
	pread(1, fd, preadbuf, *tc->size, *tc->offset);
	for (i = 0; i < *tc->size; i++) {
		if (preadbuf[i] != 0x61)
			break;
	}
	if (i != *tc->size) {
		tst_res(TFAIL, "Buffer wrong at %i have %02x expected 0x61",
			 i, preadbuf[i]);
		return;
	}
	tst_res(TPASS, "pwritev(O_DIRECT) wrote %zi bytes successfully "
		 "with content 'a' expectedly ", *tc->size);
}

void setup(void)
{
	int dev_fd, ret;
	
	dev_fd = open(tst_device->dev, O_RDWR);
	ioctl(dev_fd, BLKSSZGET, &ret);
	close(dev_fd);
	if (ret <= 0)
		tst_brk(TBROK, "BLKSSZGET returned invalid block size %i", ret);
	tst_res(TINFO, "Using block size %i", ret);
	blksz = ret;
	blk_off = ret;
	fd = open(FNAME, O_RDWR | O_CREAT | O_DIRECT, 0644);
	initbuf = memalign(blksz, blksz * 2);
	memset(initbuf, 0x00, blksz * 2);
	preadbuf = memalign(blksz, blksz);
	wr_iovec[0].iov_base = memalign(blksz, blksz);
	wr_iovec[0].iov_len = blksz;
	memset(wr_iovec[0].iov_base, 0x61, blksz);
}

void cleanup(void)
{
	free(initbuf);
	free(preadbuf);
	free(wr_iovec[0].iov_base);
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
