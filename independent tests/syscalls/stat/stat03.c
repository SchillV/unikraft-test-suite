#include "incl.h"
#define TST_EACCES_DIR   "tst_eaccesdir"
#define TST_EACCES_FILE  "tst_eaccesdir/tst"
#define TST_ENOENT       "tst_enoent/tst"
#define TST_ENOTDIR_DIR  "tst_enotdir/tst"
#define TST_ENOTDIR_FILE "tst_enotdir"
#define MODE_RW	        0666
#define DIR_MODE        0755

struct passwd *ltpuser;

char long_dir[PATH_MAX + 2] = {[0 ... PATH_MAX + 1] = 'a'};

char loop_dir[PATH_MAX] = ".";

struct tcase{
	char *pathname;
	int exp_errno;
} TC[] = {
	{TST_EACCES_FILE, EACCES},
	{NULL, EFAULT},
	{long_dir, ENAMETOOLONG},
	{TST_ENOENT, ENOENT},
	{TST_ENOTDIR_DIR, ENOTDIR},
	{loop_dir, ELOOP}
};

int  verify_stat(unsigned int n)
{
	struct tcase *tc = TC + n;
	struct stat stat_buf;
	TST_EXP_FAIL(stat(tc->pathname, &stat_buf), tc->exp_errno);
}

void setup(void)
{
	unsigned int i;
	ltpuser = getpwnam("nobody");
	setuid(ltpuser->pw_uid);
	mkdir(TST_EACCES_DIR, DIR_MODE);
	touch(TST_EACCES_FILE, DIR_MODE, NULL);
	chmod(TST_EACCES_DIR, MODE_RW);
	for (i = 0; i < ARRAY_SIZE(TC); i++) {
		if (TC[i].exp_errno == EFAULT)
			TC[i].pathname = tst_get_bad_addr(NULL);
	}
	touch(TST_ENOTDIR_FILE, DIR_MODE, NULL);
	mkdir("test_eloop", DIR_MODE);
	symlink("../test_eloop", "test_eloop/test_eloop");
	for (i = 0; i < 43; i++)
		strcat(loop_dir, "/test_eloop");
}

void main(){
	setup();
	cleanup();
}
