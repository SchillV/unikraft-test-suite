#include "incl.h"
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#define MNTPOINT	"mntpoint"
#define TEST_FILE	MNTPOINT "/file"
#define TRUSTED_BIG	"trusted.big"
#define TRUSTED_SMALL	"trusted.small"

volatile int end;

char big_value[512];

char small_value[32];

void sigproc(int sig)
{
	end = sig;
}

void loop_getxattr(void)
{
	int res;
	while (!end) {
		res = getxattr(TEST_FILE, TRUSTED_SMALL, NULL, 0);
		if (res == -1) {
			if (errno == ENODATA) {
				tst_res(TFAIL, "getxattr() failed to get an "
					"existing attribute");
			} else {
				tst_res(TFAIL | TERRNO,
					"getxattr() failed without ENOATTR");
			}
			exit(0);
		}
	}
	tst_res(TPASS, "getxattr() succeeded to get an existing attribute");
	exit(0);
}

int  verify_getxattr(
{
	pid_t pid;
	int n;
	end = 0;
	pid = fork();
	if (!pid)
		loop_getxattr();
	for (n = 0; n < 99; n++) {
		setxattr(TEST_FILE, TRUSTED_BIG, big_value,
				sizeof(big_value), XATTR_CREATE);
		removexattr(TEST_FILE, TRUSTED_BIG);
	}
	kill(pid, SIGUSR1);
	tst_reap_children();
}

void setup(void)
{
	signal(SIGUSR1, sigproc);
	touch(TEST_FILE, 0644, NULL);
	memset(big_value, 'a', sizeof(big_value));
	memset(small_value, 'a', sizeof(small_value));
	setxattr(TEST_FILE, TRUSTED_SMALL, small_value,
			sizeof(small_value), XATTR_CREATE);
}

void main(){
	setup();
	cleanup();
}
