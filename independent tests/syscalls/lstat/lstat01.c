#include "incl.h"
#define TESTFILE        "tst_file"
#define TESTSYML        "tst_syml"

uid_t user_id;

gid_t group_id;

void run(void)
{
	struct stat stat_buf;
	memset(&stat_buf, 0, sizeof(stat_buf));
lstat(TESTSYML, &stat_buf);
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "Calling lstat() failed");
	if ((stat_buf.st_mode & S_IFMT) != S_IFLNK ||
	    stat_buf.st_uid != user_id ||
	    stat_buf.st_gid != group_id ||
	    stat_buf.st_size != strlen(TESTFILE)) {
		tst_res(TFAIL,
			"lstat() reported incorrect values for the symlink!");
	} else {
		tst_res(TPASS,
			"lstat() reported correct values for the symlink!");
	}
}

void setup(void)
{
	user_id  = getuid();
	group_id = getgid();
	touch(TESTFILE, 0644, NULL);
	symlink(TESTFILE, TESTSYML);
}

void cleanup(void)
{
	unlink(TESTSYML);
}

void main(){
	setup();
	cleanup();
}
