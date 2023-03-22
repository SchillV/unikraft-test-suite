#include "incl.h"
#define DIR_MODE	(S_IRWXU | S_IRWXG | S_IRWXO)
#define FILE_MODE	(S_IRWXU | S_IRWXG | S_IRWXO)
#define TESTDIR		"testdir"
#define TESTDIR2	"nosuchdir/testdir2"
#define TESTDIR3	"testfile2/testdir3"
#define TESTDIR4	"/loopdir"
#define MNT_POINT	"mntpoint"
#define TESTDIR5	"mntpoint/dir"
#define TESTFILE    "testdir/testfile"
#define TESTFILE2   "testfile2"

char longpathname[PATH_MAX + 2];

char looppathname[sizeof(TESTDIR4) * 43] = ".";

struct testcase {
	char *dir;
	int exp_errno;
} tcases[] =  {
	{TESTDIR, ENOTEMPTY},
	{longpathname, ENAMETOOLONG},
	{TESTDIR2, ENOENT},
	{TESTDIR3, ENOTDIR},
	{NULL, EFAULT},
	{looppathname, ELOOP},
	{TESTDIR5, EROFS},
	{MNT_POINT, EBUSY},
	{".", EINVAL}
};

void setup(void)
{
	unsigned int i;
	mkdir(TESTDIR, DIR_MODE);
	touch(TESTFILE, FILE_MODE, NULL);
	memset(longpathname, 'a', PATH_MAX + 1);
	touch(TESTFILE2, FILE_MODE, NULL);
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		if (!tcases[i].dir)
			tcases[i].dir = tst_get_bad_addr(NULL);
	}
	 * NOTE: the ELOOP test is written based on that the
	 * consecutive symlinks limit in kernel is hardwired
	 * to 40.
	 */
	mkdir("loopdir", DIR_MODE);
	symlink("../loopdir", "loopdir/loopdir");
	for (i = 0; i < 43; i++)
		strcat(looppathname, TESTDIR4);
}

int  verify_rmdir(unsigned int n)
{
	struct testcase *tc = &tcases[n];
rmdir(tc->dir);
	if (TST_RET != -1) {
		tst_res(TFAIL, "rmdir() succeeded unexpectedly (%li)",
			TST_RET);
		return;
	}
	if (TST_ERR == tc->exp_errno) {
		tst_res(TPASS | TTERRNO, "rmdir() failed as expected");
		return;
	}
	tst_res(TFAIL | TTERRNO,
		"rmdir() failed unexpectedly; expected: %d - %s",
		tc->exp_errno, tst_strerrno(tc->exp_errno));
}

void main(){
	setup();
	cleanup();
}
