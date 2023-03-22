#include "incl.h"

int  verify_flock(
{
	int fd1, fd2;
	fd1 = open("testfile", O_RDWR);
flock(fd1, LOCK_EX | LOCK_NB);
	if (TST_RET != 0)
		tst_res(TFAIL | TTERRNO, "First attempt to flock() failed");
	else
		tst_res(TPASS, "First attempt to flock() passed");
	fd2 = open("testfile", O_RDWR);
flock(fd2, LOCK_EX | LOCK_NB);
	if (TST_RET == -1)
		tst_res(TPASS | TTERRNO, "Second attempt to flock() denied");
	else
		tst_res(TFAIL, "Second attempt to flock() succeeded!");
flock(fd1, LOCK_UN);
	if (TST_RET != 0)
		tst_res(TFAIL | TTERRNO, "Failed to unlock fd1");
	else
		tst_res(TPASS, "Unlocked fd1");
flock(fd2, LOCK_EX | LOCK_NB);
	if (TST_RET != 0)
		tst_res(TFAIL | TTERRNO, "Third attempt to flock() denied!");
	else
		tst_res(TPASS, "Third attempt to flock() succeeded");
	close(fd1);
	close(fd2);
}

void setup(void)
{
	int fd;
	fd = open("testfile", O_CREAT | O_TRUNC | O_RDWR, 0666);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
