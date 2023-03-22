#include "incl.h"
#define  FILENAME  "setuid04_testfile"

void dosetuid(void);

int  verify_setuid(
{
	pid_t pid;
	pid = fork();
	if (!pid)
		dosetuid();
	else
		dosetuid();
}

void dosetuid(void)
{
	int tst_fd;
tst_fd = open(FILENAME, O_RDWR);
	if (TST_RET != -1) {
		tst_res(TFAIL, "open() succeeded unexpectedly");
		close(tst_fd);
		return;
	}
	if (TST_ERR == EACCES)
		tst_res(TPASS, "open() returned errno EACCES");
	else
		tst_res(TFAIL | TTERRNO, "open() returned unexpected errno");
}

void setup(void)
{
	struct passwd *pw;
	uid_t uid;
	pw = getpwnam("nobody");
	uid = pw->pw_uid;
	UID16_CHECK(uid, setuid);
	touch(FILENAME, 0644, NULL);
	if (SETUID(uid) == -1) {
		tst_brk(TBROK,
			"setuid() failed to set the effective uid to %d", uid);
	}
}

void main(){
	setup();
	cleanup();
}
