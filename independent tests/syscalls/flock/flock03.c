#include "incl.h"

void childfunc(int fd)
{
	int fd2;
	TST_CHECKPOINT_WAIT(0);
	fd2 = open("testfile", O_RDWR);
	if (flock(fd2, LOCK_EX | LOCK_NB) != -1)
		tst_brk(TBROK, "CHILD: The file was not already locked");
flock(fd, LOCK_UN);
	if (TST_RET == -1) {
		tst_res(TFAIL, "CHILD: Unable to unlock file locked by "
			"parent: %s", tst_strerrno(TST_ERR));
		exit(1);
	} else {
		tst_res(TPASS, "CHILD: File locked by parent unlocked");
	}
flock(fd2, LOCK_EX | LOCK_NB);
	if (TST_RET == -1) {
		tst_res(TFAIL, "CHILD: Unable to unlock file after "
			"unlocking: %s", tst_strerrno(TST_ERR));
		exit(1);
	} else {
		tst_res(TPASS, "Locking after unlock passed");
	}
	close(fd);
	close(fd2);
	exit(0);
}

int  verify_flock(
{
	int fd1;
	pid_t pid;
	fd1 = open("testfile", O_RDWR);
	pid = fork();
	if (pid == 0)
		childfunc(fd1);
flock(fd1, LOCK_EX | LOCK_NB);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO,
			"Parent: Initial attempt to flock() failed");
	} else {
		tst_res(TPASS,
			"Parent: Initial attempt to flock() passed");
	}
	TST_CHECKPOINT_WAKE(0);
	tst_reap_children();
	close(fd1);
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
