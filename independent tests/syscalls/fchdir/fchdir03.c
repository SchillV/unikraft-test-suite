#include "incl.h"
#define DIRNAME "fchdir03_dir"

int fd;
int  verify_fchdir(
{
fchdir(fd);
	if (TST_RET != -1) {
		tst_res(TFAIL, "fchdir() succeeded unexpectedly");
		return;
	}
	if (TST_ERR != EACCES) {
		tst_res(TFAIL | TTERRNO, "fchdir() should fail with EACCES");
		return;
	}
	tst_res(TPASS | TTERRNO, "fchdir() failed expectedly");
}
void setup(void)
{
	struct passwd *pw;
	pw = getpwnam("nobody");
	seteuid(pw->pw_uid);
	mkdir(DIRNAME, 0400);
	fd = open(DIRNAME, O_RDONLY);
}

void main(){
	setup();
	cleanup();
}
