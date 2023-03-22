#include "incl.h"
#define DIRNAME "testdir"
#define FILENAME DIRNAME"/file1"

uid_t nobody_uid;

struct tcase {
	const char *fname;
} tcases[] = {
	{DIRNAME "/file"},
	{FILENAME}
};

void child_fn(unsigned int i)
{
	seteuid(nobody_uid);
creat(tcases[i].fname, 0444);
	if (TST_RET != -1) {
		unlink(tcases[i].fname);
		tst_res(TFAIL, "call succeeded unexpectedly");
		return;
	}
	if (TST_ERR != EACCES) {
		tst_res(TFAIL | TTERRNO, "Expected EACCES");
		return;
	}
	tst_res(TPASS, "call failed with EACCES as expected");
}

int  verify_creat(unsigned int i)
{
	if (fork() == 0)
		child_fn(i);
}

void setup(void)
{
	struct passwd *pw;
	int fd;
	pw = getpwnam("nobody");
	nobody_uid = pw->pw_uid;
	mkdir(DIRNAME, 0700);
	fd = open(FILENAME, O_RDWR | O_CREAT, 0444);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
