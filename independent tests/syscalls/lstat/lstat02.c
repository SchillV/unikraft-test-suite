#include "incl.h"
#define MODE_RWX	0777
#define MODE_RW0	0666
#define TEST_DIR	"test_dir"
#define TEST_FILE	"test_file"
#define TEST_ELOOP	"/test_eloop"
#define TEST_ENOENT	""
#define TEST_EACCES	TEST_DIR"/test_eacces"
#define TEST_ENOTDIR	TEST_FILE"/test_enotdir"

char longpathname[PATH_MAX + 2];

char elooppathname[sizeof(TEST_ELOOP) * 43];

struct stat stat_buf;

struct test_case_t {
	char *pathname;
	int exp_errno;
} test_cases[] = {
	{TEST_EACCES, EACCES},
	{TEST_ENOENT, ENOENT},
	{NULL, EFAULT},
	{longpathname, ENAMETOOLONG},
	{TEST_ENOTDIR, ENOTDIR},
	{elooppathname, ELOOP},
};

void run(unsigned int n)
{
	struct test_case_t *tc = &test_cases[n];
lstat(tc->pathname, &stat_buf);
	if (TST_RET != -1) {
		tst_res(TFAIL | TTERRNO, "lstat() returned %ld, expected -1",
			TST_RET);
		return;
	}
	if (tc->exp_errno == TST_ERR) {
		tst_res(TPASS | TTERRNO, "lstat() failed as expected");
	} else {
		tst_res(TFAIL | TTERRNO,
			"lstat() failed unexpectedly; expected: %s - got",
			tst_strerrno(tc->exp_errno));
	}
}

void setup(void)
{
	int i;
	struct passwd *ltpuser;
	if (geteuid() == 0) {
		ltpuser = getpwnam("nobody");
		seteuid(ltpuser->pw_uid);
	}
	memset(longpathname, 'a', PATH_MAX+1);
	longpathname[PATH_MAX+1] = '\0';
	mkdir(TEST_DIR, MODE_RWX);
	touch(TEST_EACCES, MODE_RWX, NULL);
	touch(TEST_FILE, MODE_RWX, NULL);
	chmod(TEST_DIR, MODE_RW0);
	mkdir("test_eloop", MODE_RWX);
	symlink("../test_eloop", "test_eloop/test_eloop");
	 * NOTE: The ELOOP test is written based on the fact that the
	 * consecutive symlinks limit in the kernel is hardwired to 40.
	 */
	elooppathname[0] = '.';
	for (i = 0; i < 43; i++)
		strcat(elooppathname, TEST_ELOOP);
}

void cleanup(void)
{
	chmod(TEST_DIR, MODE_RWX);
}

void main(){
	setup();
	cleanup();
}
