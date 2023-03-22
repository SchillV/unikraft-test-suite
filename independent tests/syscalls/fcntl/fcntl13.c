#include "incl.h"
#define F_BADCMD 999

struct flock flock;

struct tcase {
	int fd;
	int cmd;
	struct flock *flock;
	char *desc;
	int exp_errno;
} tcases[] = {
	{1, F_SETLK, NULL, "F_SETLK", EFAULT},
	{1, F_BADCMD, &flock, "F_BADCMD", EINVAL},
	{1, F_SETLK, &flock,  "F_SETLK", EINVAL},
	{-1, F_GETLK, &flock, "F_GETLK", EBADF}
};

int  verify_fcntl(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (!tc->flock)
		tc->flock = tst_get_bad_addr(NULL);
	TST_EXP_FAIL2(fcntl(tc->fd, tc->cmd, tc->flock), tc->exp_errno,
		"fcntl(%d, %s, flock)", tc->fd, tc->desc);
}

void setup(void)
{
	flock.l_whence = -1;
	flock.l_type = F_WRLCK;
	flock.l_start = 0L;
	flock.l_len = 0L;
}

void main(){
	setup();
	cleanup();
}
