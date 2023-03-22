#include "incl.h"
#define TFILE "tfile"
#define TFIFO1 "tfifo1"
#define TFIFO2 "tfifo2"

int bad_fd = -1;

int fd, pfd1, pfd2;

int pfds[2];

struct tcase {
	int *fd;
	int whence;
	int exp_err;
} tcases[] = {
	{&bad_fd, SEEK_SET, EBADF},
	{&bad_fd, SEEK_CUR, EBADF},
	{&bad_fd, SEEK_END, EBADF},
	{&fd, 5, EINVAL},
	{&fd, -1, EINVAL},
	{&fd, 7, EINVAL},
	{&pfd1, SEEK_SET, ESPIPE},
	{&pfd1, SEEK_CUR, ESPIPE},
	{&pfd1, SEEK_END, ESPIPE},
	{&pfds[0], SEEK_SET, ESPIPE},
	{&pfds[0], SEEK_CUR, ESPIPE},
	{&pfds[0], SEEK_END, ESPIPE},
	{&pfd2, SEEK_SET, ESPIPE},
	{&pfd2, SEEK_CUR, ESPIPE},
	{&pfd2, SEEK_END, ESPIPE},
};

int  verify_lseek(unsigned int n)
{
	struct tcase *tc = &tcases[n];
lseek(*tc->fd, (off_t) 1, tc->whence);
	if (TST_RET != (off_t) -1) {
		tst_res(TFAIL, "lseek(%d, 1, %d) succeeded unexpectedly",
			*tc->fd, tc->whence);
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
	fd = open(TFILE, O_RDWR | O_CREAT, 0777);
	mkfifo(TFIFO1, 0777);
	pfd1 = open(TFIFO1, O_RDWR, 0777);
	pipe(pfds);
	mknod(TFIFO2, S_IFIFO | 0777, 0);
	pfd2 = open(TFIFO2, O_RDWR, 0777);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	if (pfd1 > 0)
		close(pfd1);
	if (pfds[0] > 0)
		close(pfds[0]);
	if (pfds[1] > 0)
		close(pfds[1]);
	if (pfd2 > 0)
		close(pfd2);
}

void main(){
	setup();
	cleanup();
}
