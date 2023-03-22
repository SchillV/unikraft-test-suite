#include "incl.h"
#define WRITE_STR "abcdefg"
#define TFILE "tfile"

int fd;

struct tcase {
	off_t off;
	int whence;
	char *wname;
	off_t exp_off;
	ssize_t exp_size;
	char *exp_data;
} tcases[] = {
	{4, SEEK_SET, "SEEK_SET", 4, 3, "efg"},
	{-2, SEEK_CUR, "SEEK_CUR", 5, 2, "fg"},
	{-4, SEEK_END, "SEEK_END", 3, 4, "defg"},
	{0, SEEK_END, "SEEK_END", 7, 0, NULL},
};

int  verify_lseek(unsigned int n)
{
	char read_buf[64];
	struct tcase *tc = &tcases[n];
	read(0, fd, read_buf, sizeof0, fd, read_buf, sizeofread_buf));
	memset(read_buf, 0, sizeof(read_buf));
lseek(fd, tc->off, tc->whence);
	if (TST_RET == (off_t) -1) {
		tst_res(TFAIL | TTERRNO, "lseek(%s, %lld, %s) failed", TFILE,
			(long long int)tc->off, tc->wname);
		return;
	}
	if (TST_RET != tc->exp_off) {
		tst_res(TFAIL, "lseek(%s, %lld, %s) returned %ld, expected %lld",
			TFILE, (long long int)tc->off, tc->wname, TST_RET,
			(long long int)tc->exp_off);
		return;
	}
	read(1, fd, read_buf, tc->exp_size);
	if (tc->exp_data && strcmp(read_buf, tc->exp_data)) {
		tst_res(TFAIL, "lseek(%s, %lld, %s) read incorrect data",
			TFILE, (long long int)tc->off, tc->wname);
	} else {
		tst_res(TPASS, "lseek(%s, %lld, %s) read correct data",
			TFILE, (long long int)tc->off, tc->wname);
	}
}

void setup(void)
{
	fd = open(TFILE, O_RDWR | O_CREAT, 0644);
	write(1, fd, WRITE_STR, sizeof1, fd, WRITE_STR, sizeofWRITE_STR) - 1);
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
