#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT	"mntpoint"
#define FNAME	MNTPOINT"/file"

int fd;

off_t blk_off, zero_off;

ssize_t blksz;

char *pop_buf;

struct iovec rd_iovec[] = {
	{NULL, 0},
	{NULL, 0},
};

struct tcase {
	int count;
	off_t *offset;
	ssize_t *size;
	char content;
} tcases[] = {
	{1, &zero_off, &blksz, 0x61},
	{2, &zero_off, &blksz, 0x61},
	{1, &blk_off, &blksz, 0x62},
};

int  verify_direct_preadv(unsigned int n)
{
	int i;
	char *vec;
	struct tcase *tc = &tcases[n];
	vec = rd_iovec[0].iov_base;
	memset(vec, 0x00, blksz);
	lseek(fd, 0, SEEK_SET);
preadv(fd, rd_iovec, tc->count, *tc->offset);
	if (TST_RET < 0) {
		tst_res(TFAIL | TTERRNO, "preadv(O_DIRECT) fails");
		return;
	}
	if (TST_RET != *tc->size) {
		tst_res(TFAIL, "preadv(O_DIRECT) read %li bytes, expected %zi",
			 TST_RET, *tc->size);
		return;
	}
	for (i = 0; i < *tc->size; i++) {
		if (vec[i] != tc->content)
			break;
	}
	if (i < *tc->size) {
		tst_res(TFAIL, "Buffer wrong at %i have %02x expected %02x",
			 i, vec[i], tc->content);
		return;
	}
	if (lseek(fd, 0, SEEK_CUR) != 0) {
		tst_res(TFAIL, "preadv(O_DIRECT) has changed file offset");
		return;
	}
	tst_res(TPASS, "preadv(O_DIRECT) read %zi bytes successfully "
		 "with content '%c' expectedly", *tc->size, tc->content);
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
	pop_buf = memalign(blksz, blksz);
	memset(pop_buf, 0x61, blksz);
	write(1, fd, pop_buf, blksz);
	memset(pop_buf, 0x62, blksz);
	write(1, fd, pop_buf, blksz);
	rd_iovec[0].iov_base = memalign(blksz, blksz);
	rd_iovec[0].iov_len = blksz;
}

void cleanup(void)
{
	free(pop_buf);
	free(rd_iovec[0].iov_base);
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
