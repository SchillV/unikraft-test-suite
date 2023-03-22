#include "incl.h"
char *TCID = "rename11";
#define MNTPOINT	"mntpoint"
#define TEST_EROFS	"mntpoint/test_erofs"
#define TEST_NEW_EROFS	"mntpoint/new_test_erofs"
#define TEST_EMLINK	"test_emlink"
#define TEST_NEW_EMLINK	"emlink_dir/testdir"
#define TEST_NEW_ELOOP	"new_test_eloop"
#define ELOPFILE	"/test_eloop"

char elooppathname[sizeof(ELOPFILE) * 43] = ".";

int max_subdirs;

const char *device;

const char *fs_type;

int mount_flag;

void cleanup(void);

void setup(void);

void test_eloop(void);

void test_erofs(void);

void test_emlink(void);

void (*testfunc[])(void) = { test_eloop, test_erofs, test_emlink };
int TST_TOTAL = ARRAY_SIZE(testfunc);
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
			(*testfunc[i])();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int i;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_require_root();
	tst_tmpdir();
	TEST_PAUSE;
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to obtain block device");
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	mkdir(cleanup, MNTPOINT, 0755);
	mount(cleanup, device, MNTPOINT, fs_type, 0, NULL);
	mount_flag = 1;
	touch(cleanup, TEST_EROFS, 0644, NULL);
	mkdir(cleanup, TEST_EMLINK, 0755);
	max_subdirs = tst_fs_fill_subdirs(cleanup, "emlink_dir");
	 * NOTE: the ELOOP test is written based on that the consecutive
	 * symlinks limits in kernel is hardwired to 40.
	 */
	mkdir(cleanup, "test_eloop", 0644);
	symlink(cleanup, "../test_eloop", "test_eloop/test_eloop");
	for (i = 0; i < 43; i++)
		strcat(elooppathname, ELOPFILE);
}

void check_and_print(int expected_errno)
{
	if (TEST_RETURN == -1) {
		if (TEST_ERRNO == expected_errno) {
			tst_resm(TPASS | TTERRNO, "failed as expected");
		} else {
			tst_resm(TFAIL | TTERRNO,
				 "failed unexpectedly; expected - %d : %s",
				 expected_errno, strerror(expected_errno));
		}
	} else {
		tst_resm(TFAIL, "rename succeeded unexpectedly");
	}
}

void test_eloop(void)
{
rename(elooppathname, TEST_NEW_ELOOP);
	check_and_print(ELOOP);
	if (TEST_RETURN == 0)
		unlink(cleanup, TEST_NEW_ELOOP);
}

void test_erofs(void)
{
	mount(cleanup, device, MNTPOINT, fs_type, MS_REMOUNT | MS_RDONLY,
		   NULL);
rename(TEST_EROFS, TEST_NEW_EROFS);
	check_and_print(EROFS);
	if (TEST_RETURN == 0)
		unlink(cleanup, TEST_NEW_EROFS);
	mount(cleanup, device, MNTPOINT, fs_type, MS_REMOUNT, NULL);
}

void test_emlink(void)
{
	if (max_subdirs == 0) {
		tst_resm(TCONF, "EMLINK test is not appropriate");
		return;
	}
rename(TEST_EMLINK, TEST_NEW_EMLINK);
	check_and_print(EMLINK);
	if (TEST_RETURN == 0)
		rmdir(cleanup, TEST_NEW_EMLINK);
}

void cleanup(void)
{
	if (mount_flag && tst_umount(MNTPOINT) < 0)
		tst_resm(TWARN | TERRNO, "umount device:%s failed", device);
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

