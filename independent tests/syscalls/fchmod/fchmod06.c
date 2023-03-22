#include "incl.h"
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#define MNT_POINT "mntpoint"

int fd1;

int fd2;

int fd3;

struct tcase {
	int *fd;
	int mode;
	int exp_errno;
} tcases[] = {
	{&fd1, 0644, EPERM},
	{&fd2, 0644, EBADF},
	{&fd3, 0644, EROFS},
};

int  verify_fchmod(unsigned int i)
{
	struct tcase *tc = &tcases[i];
fchmod(*tc->fd, tc->mode);
	if (TST_RET != -1) {
		tst_res(TFAIL, "fchmod() passed unexpectedly (%li)",
			TST_RET);
		return;
	}
	if (TST_ERR == tcases[i].exp_errno) {
		tst_res(TPASS | TTERRNO, "fchmod() failed expectedly");
		return;
	}
	tst_res(TFAIL | TTERRNO,
		"fchmod() failed unexpectedly, expected %i - %s",
		TST_ERR, tst_strerrno(TST_ERR));
}

void setup(void)
{
	struct passwd *ltpuser = getpwnam("nobody");
	fd3 = open(MNT_POINT"/file", O_RDONLY);
	fd1 = open("tfile_1", O_RDWR | O_CREAT, 0666);
	fd2 = open("tfile_2", O_RDWR | O_CREAT, 0666);
	close(fd2);
	seteuid(ltpuser->pw_uid);
}

void cleanup(void)
{
	if (fd1 > 0)
		close(fd1);
	if (fd3 > 0)
		close(fd3);
}

void main(){
	setup();
	cleanup();
}
