#include "incl.h"
#define TST_EEXIST	"tst_eexist"
#define TST_ENOENT	"tst_enoent/tst"
#define TST_ENOTDIR_FILE "tst_enotdir"
#define TST_ENOTDIR_DIR	"tst_enotdir/tst"
#define MODE		0777
#define MNT_POINT	"mntpoint"
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
			 S_IXGRP|S_IROTH|S_IXOTH)
#define TST_EROFS      "mntpoint/tst_erofs"

char long_dir[PATH_MAX + 2] = {[0 ... PATH_MAX + 1] = 'a'};

char loop_dir[PATH_MAX] = ".";
struct tcase;

struct tcase {
	char *pathname;
	int exp_errno;
} TC[] = {
	{NULL, EFAULT},
	{long_dir, ENAMETOOLONG},
	{TST_EEXIST, EEXIST},
	{TST_ENOENT, ENOENT},
	{TST_ENOTDIR_DIR, ENOTDIR},
	{loop_dir, ELOOP},
	{TST_EROFS, EROFS},
};

int  verify_mkdir(unsigned int n)
{
	struct tcase *tc = TC + n;
mkdir(tc->pathname, MODE);
	if (TST_RET != -1) {
		tst_res(TFAIL, "mkdir() returned %ld, expected -1, errno=%d",
			TST_RET, tc->exp_errno);
		return;
	}
	if (TST_ERR == tc->exp_errno) {
		tst_res(TPASS | TTERRNO, "mkdir() failed as expected");
	} else {
		tst_res(TFAIL | TTERRNO,
			"mkdir() failed unexpectedly; expected: %d - %s",
			 tc->exp_errno, strerror(tc->exp_errno));
	}
}

void setup(void)
{
	unsigned int i;
	touch(TST_EEXIST, MODE, NULL);
	touch(TST_ENOTDIR_FILE, MODE, NULL);
	for (i = 0; i < ARRAY_SIZE(TC); i++) {
		if (TC[i].exp_errno == EFAULT)
			TC[i].pathname = tst_get_bad_addr(NULL);
	}
	mkdir("test_eloop", DIR_MODE);
	symlink("../test_eloop", "test_eloop/test_eloop");
	for (i = 0; i < 43; i++)
		strcat(loop_dir, "/test_eloop");
}

void main(){
	setup();
	cleanup();
}
