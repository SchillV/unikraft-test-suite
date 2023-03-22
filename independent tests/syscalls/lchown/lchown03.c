#include "incl.h"
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
			 S_IXGRP|S_IROTH|S_IXOTH)
#define TEST_EROFS	"mntpoint"

char test_eloop[PATH_MAX] = ".";

const char *device;

int mount_flag;

struct test_case_t {
	char *pathname;
	int exp_errno;
} test_cases[] = {
	{test_eloop, ELOOP},
	{TEST_EROFS, EROFS},
};
TCID_DEFINE(lchown03);
int TST_TOTAL = ARRAY_SIZE(test_cases);

void setup(void);

int  lchown_verify(const struct test_case_t *);

void cleanup(void);
int main(int argc, char *argv[])
{
	int lc;
	int i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			lchown_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int i;
	const char *fs_type;
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to acquire device");
	mkdir(cleanup, "test_eloop", DIR_MODE);
	symlink(cleanup, "../test_eloop", "test_eloop/test_eloop");
	for (i = 0; i < 43; i++)
		strcat(test_eloop, "/test_eloop");
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	mkdir(cleanup, TEST_EROFS, DIR_MODE);
	mount(cleanup, device, TEST_EROFS, fs_type, MS_RDONLY, NULL);
	mount_flag = 1;
}

int  lchown_verify(const struct test_case_t *test)
{
	UID16_CHECK(geteuid(), "lchown", cleanup)
	GID16_CHECK(getegid(), "lchown", cleanup)
LCHOWN(cleanup, test->pathname, geteuid(), getegid());
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL, "lchown() returned %ld, expected -1, errno=%d",
			 TEST_RETURN, test->exp_errno);
		return;
	}
	if (TEST_ERRNO == test->exp_errno) {
		tst_resm(TPASS | TTERRNO, "lchown() failed as expected");
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "lchown() failed unexpectedly; expected: %d - %s",
			 test->exp_errno,
			 strerror(test->exp_errno));
	}
}

void cleanup(void)
{
	if (mount_flag && tst_umount(TEST_EROFS) < 0)
		tst_resm(TWARN | TERRNO, "umount device:%s failed", device);
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

