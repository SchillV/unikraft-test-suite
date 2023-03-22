#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT "mntpoint"
#define TESTDIR "testdir"
#define NEW_TESTDIR "new_testdir"
#define TESTDIR2 "/loopdir"
#define NEW_TESTDIR2 "newloopdir"
#define TESTDIR3 "emlinkdir"
#define NEW_TESTDIR3 "testemlinkdir/new_emlinkdir"
#define TESTFILE "testfile"
#define NEW_TESTFILE "new_testfile"
#define TESTFILE2 "testfile2"
#define NEW_TESTFILE2 "new_testfile2"
#define TESTFILE3 "testdir/testfile"
#define TESTFILE4 "testfile4"
#define TESTFILE5 "mntpoint/rofile"
#define NEW_TESTFILE5 "mntpoint/newrofile"
#define DIRMODE (S_IRWXU | S_IRWXG | S_IRWXO)
#define FILEMODE (S_IRWXU | S_IRWXG | S_IRWXO)

int curfd = AT_FDCWD;

int olddirfd;

int newdirfd;

int badfd = 100;

int filefd;

char absoldpath[256];

char absnewpath[256];

char looppathname[sizeof(TESTDIR2) * 43] = ".";

int max_subdirs;

int mount_flag;

const char *device;

struct test_case_t {
	int *oldfdptr;
	const char *oldpath;
	int *newfdptr;
	const char *newpath;
	int exp_errno;
} test_cases[] = {
	{ &curfd, TESTFILE, &curfd, NEW_TESTFILE, 0 },
	{ &olddirfd, TESTFILE, &newdirfd, NEW_TESTFILE, 0 },
	{ &olddirfd, absoldpath, &newdirfd, absnewpath, 0 },
	{ &badfd, TESTFILE, &badfd, NEW_TESTFILE, EBADF },
	{ &filefd, TESTFILE, &filefd, NEW_TESTFILE, ENOTDIR },
	{ &curfd, looppathname, &curfd, NEW_TESTDIR2, ELOOP },
	{ &curfd, TESTFILE5, &curfd, NEW_TESTFILE5, EROFS },
	{ &curfd, TESTDIR3, &curfd, NEW_TESTDIR3, EMLINK },
};

void setup(void);

void cleanup(void);

int  renameat_verify(const struct test_case_t *);
char *TCID = "renameat01";
int TST_TOTAL = ARRAY_SIZE(test_cases);
int main(int ac, char **av)
{
	int i, lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			renameat_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	char *tmpdir;
	const char *fs_type;
	int i;
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to obtain block device");
	TEST_PAUSE;
	touch(cleanup, TESTFILE, FILEMODE, NULL);
	touch(cleanup, TESTFILE2, FILEMODE, NULL);
	tmpdir = tst_get_tmpdir();
	sprintf(absoldpath, "%s/%s", tmpdir, TESTFILE2);
	sprintf(absnewpath, "%s/%s", tmpdir, NEW_TESTFILE2);
	free(tmpdir);
	mkdir(cleanup, TESTDIR, DIRMODE);
	touch(cleanup, TESTFILE3, FILEMODE, NULL);
	mkdir(cleanup, NEW_TESTDIR, DIRMODE);
	olddirfd = open(cleanup, TESTDIR, O_DIRECTORY);
	newdirfd = open(cleanup, NEW_TESTDIR, O_DIRECTORY);
	filefd = open(cleanup, TESTFILE4,
				O_RDWR | O_CREAT, FILEMODE);
	 * NOTE: the ELOOP test is written based on that the
	 * consecutive symlinks limit in kernel is hardwired
	 * to 40.
	 */
	mkdir(cleanup, "loopdir", DIRMODE);
	symlink(cleanup, "../loopdir", "loopdir/loopdir");
	for (i = 0; i < 43; i++)
		strcat(looppathname, TESTDIR2);
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	mkdir(cleanup, MNTPOINT, DIRMODE);
	mount(cleanup, device, MNTPOINT, fs_type, 0, NULL);
	mount_flag = 1;
	touch(cleanup, TESTFILE5, FILEMODE, NULL);
	mount(cleanup, device, MNTPOINT, fs_type, MS_REMOUNT | MS_RDONLY,
		   NULL);
	mkdir(cleanup, TESTDIR3, DIRMODE);
	max_subdirs = tst_fs_fill_subdirs(cleanup, "testemlinkdir");
}

int  renameat_verify(const struct test_case_t *tc)
{
	if (tc->exp_errno == EMLINK && max_subdirs == 0) {
		tst_resm(TCONF, "EMLINK test is not appropriate");
		return;
	}
renameat(*(tc->oldfdptr), tc->oldpat;
			*(tc->newfdptr), tc->newpath));
	if (tc->exp_errno && TEST_RETURN != -1) {
		tst_resm(TFAIL, "renameat() succeeded unexpectedly");
		return;
	}
	if (tc->exp_errno == 0 && TEST_RETURN != 0) {
		tst_resm(TFAIL | TTERRNO, "renameat() failed unexpectedly");
		return;
	}
	if (TEST_ERRNO == tc->exp_errno) {
		tst_resm(TPASS | TTERRNO,
		"renameat() returned the expected value");
	} else {
		tst_resm(TFAIL | TTERRNO,
			"renameat() got unexpected return value; expected: "
			"%d - %s", tc->exp_errno,
			strerror(tc->exp_errno));
	}
	if (TEST_ERRNO == 0 && renameat(*(tc->newfdptr), tc->newpath,
		*(tc->oldfdptr), tc->oldpath) < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "renameat(%d, %s, "
			"%d, %s) failed.", *(tc->newfdptr), tc->newpath,
			*(tc->oldfdptr), tc->oldpath);
	}
}

void cleanup(void)
{
	if (olddirfd > 0 && close(olddirfd) < 0)
		tst_resm(TWARN | TERRNO, "close olddirfd failed");
	if (newdirfd > 0 && close(newdirfd) < 0)
		tst_resm(TWARN | TERRNO, "close newdirfd failed");
	if (filefd > 0 && close(filefd) < 0)
		tst_resm(TWARN | TERRNO, "close filefd failed");
	if (mount_flag && tst_umount(MNTPOINT) < 0)
		tst_resm(TWARN | TERRNO, "umount %s failed", MNTPOINT);
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

