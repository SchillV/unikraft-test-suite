#include "incl.h"
#define TFILE1 "tfile1"
#define TFILE2 "tfile2"
#define WR_STR1 "abcdefg"
#define WR_STR2 "ijk"

int fd1, fd2;

struct tcase {
	int *fd;
	char *fname;
	off_t off;
	off_t exp_off;
	int exp_size;
	char *exp_data;
} tcases[] = {
	{&fd1, TFILE1, 7, 7, 10, "abcdefgijk"},
	{&fd2, TFILE2, 2, 2, 7, "abijkfg"},
};

int  verify_lseek(unsigned int n)
{
	char read_buf[64];
	struct tcase *tc = &tcases[n];
	memset(read_buf, 0, sizeof(read_buf));
lseek(*tc->fd, tc->off, SEEK_SET);
	if (TST_RET == (off_t) -1) {
		tst_res(TFAIL | TTERRNO, "lseek(%s, %lld, SEEK_SET) failed",
			tc->fname, (long long int)tc->off);
		return;
	}
	if (TST_RET != tc->exp_off) {
		tst_res(TFAIL, "lseek(%s, %lld, SEEK_SET) returned %ld, expected %lld",
			tc->fname, (long long int)tc->off, TST_RET,
			(long long int)tc->exp_off);
		return;
	}
	write(1, *tc->fd, WR_STR2, sizeof1, *tc->fd, WR_STR2, sizeofWR_STR2) - 1);
	close(*tc->fd);
	read(1, *tc->fd, read_buf, tc->exp_size);
	if (strcmp(read_buf, tc->exp_data)) {
		tst_res(TFAIL, "lseek(%s, %lld, SEEK_SET) wrote incorrect data %s",
			tc->fname, (long long int)tc->off, read_buf);
	} else {
		tst_res(TPASS, "lseek(%s, %lld, SEEK_SET) wrote correct data %s",
			tc->fname, (long long int)tc->off, read_buf);
	}
}

void setup(void)
{
	fd1 = open(TFILE1, O_RDWR | O_CREAT, 0644);
	fd2 = open(TFILE2, O_RDWR | O_CREAT, 0644);
	write(1, fd1, WR_STR1, sizeof1, fd1, WR_STR1, sizeofWR_STR1) - 1);
	write(1, fd2, WR_STR1, sizeof1, fd2, WR_STR1, sizeofWR_STR1) - 1);
}

void cleanup(void)
{
	if (fd1 > 0)
		close(fd1);
	if (fd2 > 0)
		close(fd2);
}

void main(){
	setup();
	cleanup();
}
