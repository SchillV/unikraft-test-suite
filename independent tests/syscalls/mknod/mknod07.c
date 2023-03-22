#include "incl.h"
#define DIR_TEMP		"testdir_1"
#define DIR_TEMP_MODE		(S_IRUSR | S_IXUSR)
#define DIR_MODE		(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
				 S_IXGRP|S_IROTH|S_IXOTH)
#define MNT_POINT		"mntpoint"
#define FIFO_MODE	(S_IFIFO | S_IRUSR | S_IRGRP | S_IROTH)
#define SOCKET_MODE	(S_IFSOCK | S_IRWXU | S_IRWXG | S_IRWXO)
#define CHR_MODE	(S_IFCHR | S_IRUSR | S_IWUSR)
#define BLK_MODE	(S_IFBLK | S_IRUSR | S_IWUSR)
#define ELOPFILE	"/test_eloop"

char elooppathname[sizeof(ELOPFILE) * 43] = ".";

const char *device;

int mount_flag;

struct test_case_t {
	char *pathname;
	int mode;
	int exp_errno;
	int major, minor;
} test_cases[] = {
	{ "testdir_1/tnode_1", SOCKET_MODE, EACCES, 0, 0 },
	{ "testdir_1/tnode_2", FIFO_MODE, EACCES, 0, 0 },
	{ "tnode_3", CHR_MODE, EPERM, 1, 3 },
	{ "tnode_4", BLK_MODE, EPERM, 0, 0 },
	{ "mntpoint/tnode_5", SOCKET_MODE, EROFS, 0, 0 },
	{ elooppathname, FIFO_MODE, ELOOP, 0, 0 },
};
char *TCID = "mknod07";
int TST_TOTAL = ARRAY_SIZE(test_cases);

void setup(void);

int  mknod_verify(const struct test_case_t *test_case);

void cleanup(void);
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			mknod_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int i;
	struct passwd *ltpuser;
	const char *fs_type;
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to acquire device");
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	TEST_PAUSE;
	mkdir(cleanup, MNT_POINT, DIR_MODE);
	mount(cleanup, device, MNT_POINT, fs_type, MS_RDONLY, NULL);
	mount_flag = 1;
	ltpuser = getpwnam(cleanup, "nobody");
	seteuid(cleanup, ltpuser->pw_uid);
	mkdir(cleanup, DIR_TEMP, DIR_TEMP_MODE);
	 * NOTE: the ELOOP test is written based on that the consecutive
	 * symlinks limits in kernel is hardwired to 40.
	 */
	mkdir(cleanup, "test_eloop", DIR_MODE);
	symlink(cleanup, "../test_eloop", "test_eloop/test_eloop");
	for (i = 0; i < 43; i++)
		strcat(elooppathname, ELOPFILE);
}

int  mknod_verify(const struct test_case_t *test_case)
{
mknod(test_case->pathname, test_case->mod;
		makedev(test_case->major, test_case->minor)));
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL, "mknod succeeded unexpectedly");
		return;
	}
	if (TEST_ERRNO == test_case->exp_errno) {
		tst_resm(TPASS | TTERRNO, "mknod failed as expected");
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "mknod failed unexpectedly; expected: "
			 "%d - %s", test_case->exp_errno,
			 strerror(test_case->exp_errno));
	}
}

void cleanup(void)
{
	if (seteuid(0) == -1)
		tst_resm(TWARN | TERRNO, "seteuid(0) failed");
	if (mount_flag && tst_umount(MNT_POINT) < 0)
		tst_resm(TWARN | TERRNO, "umount device:%s failed", device);
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

