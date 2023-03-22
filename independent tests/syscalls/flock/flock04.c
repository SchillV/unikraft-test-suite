#include "incl.h"

struct tcase {
	int operation;
	char *f_lock;
} tcases[] = {
	{LOCK_SH, "shared lock"},
	{LOCK_EX, "exclusive lock"},
};

void child(int opt, int should_pass, char *lock)
{
	int retval, fd1;
	fd1 = open("testfile", O_RDWR);
	retval = flock(fd1, opt);
	if (should_pass) {
		tst_res(retval == -1 ? TFAIL : TPASS,
			" Child acquiring %s got %d", lock, retval);
	} else {
		tst_res(retval == -1 ? TPASS : TFAIL,
			" Child acquiring %s got %d", lock, retval);
	}
	close(fd1);
	exit(0);
}

int  verify_flock(unsigned n)
{
	int fd2;
	pid_t pid;
	struct tcase *tc = &tcases[n];
	fd2 = open("testfile", O_RDWR);
flock(fd2, tc->operation);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "flock() failed to acquire %s",
			tc->f_lock);
		close(fd2);
		return;
	}
	tst_res(TPASS, "Parent had %s", tc->f_lock);
	pid = fork();
	if (pid == 0)
		child(LOCK_SH | LOCK_NB, tc->operation & LOCK_SH, "shared lock");
	else
		tst_reap_children();
	pid = fork();
	if (pid == 0)
		child(LOCK_EX | LOCK_NB, 0, "exclusive lock");
	else
		tst_reap_children();
	close(fd2);
}

void setup(void)
{
	int fd;
	fd = open("testfile", O_CREAT | O_TRUNC | O_RDWR, 0644);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
