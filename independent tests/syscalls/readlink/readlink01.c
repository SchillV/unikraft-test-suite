#include "incl.h"
#define TESTFILE "test_file"
#define SYMFILE	"slink_file"

uid_t nobody_uid;

void test_readlink(void)
{
	char buffer[256];
	int exp_val = strlen(TESTFILE);
readlink(SYMFILE, buffer, sizeof(buffer));
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "readlink() on %s failed", SYMFILE);
		return;
	}
	if (TST_RET != exp_val) {
		tst_res(TFAIL, "readlink() returned value %ld "
			"did't match, Expected %d", TST_RET, exp_val);
		return;
	}
	if (memcmp(buffer, TESTFILE, exp_val) != 0) {
		tst_res(TFAIL, "Pathname %s and buffer contents %s differ",
			TESTFILE, buffer);
	} else {
		tst_res(TPASS, "readlink() functionality on '%s' was correct",
			SYMFILE);
	}
}

int  verify_readlink(unsigned int n)
{
	pid_t pid;
	if (n) {
		tst_res(TINFO, "Running test as nobody");
		pid = fork();
		if (!pid) {
			setuid(nobody_uid);
			test_readlink();
			return;
		}
	} else {
		tst_res(TINFO, "Running test as root");
		test_readlink();
	}
}

void setup(void)
{
	int fd;
	struct passwd *pw;
	pw = getpwnam("nobody");
	nobody_uid = pw->pw_uid;
	fd = open(TESTFILE, O_RDWR | O_CREAT, 0444);
	close(fd);
	symlink(TESTFILE, SYMFILE);
}

void main(){
	setup();
	cleanup();
}
