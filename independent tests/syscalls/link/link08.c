#include "incl.h"
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
			 S_IXGRP|S_IROTH|S_IXOTH)
#define MNT_POINT	"mntpoint"
#define TEST_FILE	"testfile"
#define TEST_FILE1	"testfile1"
#define TEST_FILE2	"mntpoint/file"
#define TEST_FILE3	"mntpoint/testfile4"

char test_file4[PATH_MAX] = ".";

void setup(void);

struct tcase {
	char *oldpath;
	char *newpath;
	int exp_errno;
} tcases[] = {
	{TEST_FILE1, TEST_FILE, EPERM},
	{TEST_FILE2, TEST_FILE, EXDEV},
	{TEST_FILE2, TEST_FILE3, EROFS},
	{test_file4, TEST_FILE, ELOOP},
};

int  link_verify(unsigned int i)
{
	struct tcase *tc = &tcases[i];
link(tc->oldpath, tc->newpath);
	if (TST_RET != -1) {
		tst_res(TFAIL, "link() succeeded unexpectedly (%li)",
			TST_RET);
		return;
	}
	if (TST_ERR == tc->exp_errno) {
		tst_res(TPASS | TTERRNO, "link() failed as expected");
		return;
	}
	tst_res(TFAIL | TTERRNO,
		"link() failed unexpectedly; expected: %d - %s",
		tc->exp_errno, tst_strerrno(tc->exp_errno));
}

void setup(void)
{
	int i;
	mkdir(TEST_FILE1, DIR_MODE);
	mkdir("test_eloop", DIR_MODE);
	symlink("../test_eloop", "test_eloop/test_eloop");
	for (i = 0; i < 43; i++)
		strcat(test_file4, "/test_eloop");
}

void main(){
	setup();
	cleanup();
}
