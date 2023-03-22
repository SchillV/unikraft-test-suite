#include "incl.h"
#define DIR_TEMP	"test_dir_1"
#define TEST_FILE1	"test_dir_1/test_file_1"
#define SYM_FILE1	"test_dir_1/slink_file_1"
#define TEST_FILE2	"test_file_2"
#define SYM_FILE2	"slink_file_2"
#define TEST_FILE3	"test_file_3"
#define SYM_FILE3	"test_file_3/slink_file_3"
#define ELOOPFILE	"/test_eloop"
#define TESTFILE	"test_file"
#define SYMFILE		"slink_file"

char longpathname[PATH_MAX + 2];

char elooppathname[sizeof(ELOOPFILE) * 43] = ".";

char buffer[256];

struct tcase {
	char *link;
	char *buf;
	size_t buf_size;
	int exp_errno;
} tcases[] = {
	{SYM_FILE1, buffer, sizeof(buffer), EACCES},
	{SYM_FILE2, buffer, 0, EINVAL},
	{TEST_FILE2, buffer, sizeof(buffer), EINVAL},
	{longpathname, buffer, sizeof(buffer), ENAMETOOLONG},
	{"", buffer, sizeof(buffer), ENOENT},
	{SYM_FILE3, buffer, sizeof(buffer), ENOTDIR},
	{elooppathname, buffer, sizeof(buffer), ELOOP},
	{SYMFILE, (char *)-1, sizeof(buffer), EFAULT},
};

int  verify_readlink(unsigned int n)
{
	struct tcase *tc = &tcases[n];
readlink(tc->link, tc->buf, tc->buf_size);
	if (TST_RET != -1) {
		tst_res(TFAIL, "readlink() sueeeeded unexpectedly");
		return;
	}
	if (TST_ERR != tc->exp_errno) {
		tst_res(TFAIL | TTERRNO,
			"readlink() failed unexpectedly; expected: %d - %s, got",
			tc->exp_errno, tst_strerrno(tc->exp_errno));
		if (tc->exp_errno == ENOENT && TST_ERR == EINVAL) {
			tst_res(TWARN | TTERRNO,
				"It may be a Kernel Bug, see the patch:"
		}
	} else {
		tst_res(TPASS | TTERRNO, "readlink() failed as expected");
	}
}

void setup(void)
{
	int i;
	struct passwd *pwent;
	pwent = getpwnam("nobody");
	seteuid(pwent->pw_uid);
	mkdir(DIR_TEMP, 0777);
	touch(TEST_FILE1, 0666, NULL);
	symlink(TEST_FILE1, SYM_FILE1);
	chmod(DIR_TEMP, 0444);
	touch(TEST_FILE2, 0666, NULL);
	symlink(TEST_FILE2, SYM_FILE2);
	memset(longpathname, 'a', PATH_MAX + 1);
	touch(TEST_FILE3, 0666, NULL);
	mkdir("test_eloop", 0777);
	symlink("../test_eloop", "test_eloop/test_eloop");
	for (i = 0; i < 43; i++)
		strcat(elooppathname, ELOOPFILE);
	touch(TESTFILE, 0666, NULL);
	symlink(TESTFILE, SYMFILE);
}

void main(){
	setup();
	cleanup();
}
