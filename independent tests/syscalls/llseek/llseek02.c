#include "incl.h"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define TEMP_FILE1 "tmp_file1"
#define TEMP_FILE2 "tmp_file2"
#define FILE_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define SEEK_TOP 10

int fd1;

int fd2;

struct tcase {
	int *fd;
	int whence;
	int exp_err;
} tcases[] = {
	{&fd1, SEEK_TOP, EINVAL},
	{&fd2, SEEK_SET, EBADF},
};

int  verify_llseek(unsigned int n)
{
	struct tcase *tc = &tcases[n];
lseek(*tc->fd, (loff_t) 1, tc->whence);
	if (TST_RET != (off_t) -1) {
		tst_res(TFAIL, "lseek(%d, 1, %d) succeeded unexpectedly (%ld)",
			*tc->fd, tc->whence, TST_RET);
		return;
	}
	if (TST_ERR == tc->exp_err) {
		tst_res(TPASS | TTERRNO, "lseek(%d, 1, %d) failed as expected",
			*tc->fd, tc->whence);
	} else {
		tst_res(TFAIL | TTERRNO, "lseek(%d, 1, %d) failed "
		        "unexpectedly, expected %s", *tc->fd, tc->whence,
		        tst_strerrno(tc->exp_err));
	}
}

void setup(void)
{
	fd1 = open(TEMP_FILE1, O_RDWR | O_CREAT, FILE_MODE);
	fd2 = open(TEMP_FILE2, O_RDWR | O_CREAT, FILE_MODE);
	close(fd2);
}

void main(){
	setup();
	cleanup();
}
