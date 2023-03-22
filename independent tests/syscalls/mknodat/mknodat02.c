#include "incl.h"
#define _GNU_SOURCE

void setup(void);

void cleanup(void);
#define DIR_MODE	(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP| \
			 S_IXGRP|S_IROTH|S_IXOTH)
#define MNT_POINT	"mntpoint"
#define FIFOMODE	(S_IFIFO | S_IRUSR | S_IRGRP | S_IROTH)
#define FREGMODE	(S_IFREG | S_IRUSR | S_IRGRP | S_IROTH)
#define SOCKMODE	(S_IFSOCK | S_IRUSR | S_IRGRP | S_IROTH)

const char *device;

int mount_flag;

int dir_fd;

int curfd = AT_FDCWD;
#define ELOPFILE	"/test_eloop"

char elooppathname[sizeof(ELOPFILE) * 43] = ".";

struct test_case_t {
	int *dir_fd;
	char *pathname;
	mode_t mode;
	int exp_errno;
} test_cases[] = {
	{ &curfd, "tnode1", FIFOMODE, 0 },
	{ &curfd, "tnode2", FREGMODE, 0 },
	{ &curfd, "tnode3", SOCKMODE, 0 },
	{ &dir_fd, "tnode4", FIFOMODE, EROFS },
	{ &dir_fd, "tnode5", FREGMODE, EROFS },
	{ &dir_fd, "tnode6", SOCKMODE, EROFS },
	{ &curfd, elooppathname, FIFOMODE, ELOOP },
	{ &curfd, elooppathname, FREGMODE, ELOOP },
	{ &curfd, elooppathname, SOCKMODE, ELOOP },
};

int  mknodat_verify(struct test_case_t *tc);
char *TCID = "mknodat";
int TST_TOTAL = ARRAY_SIZE(test_cases);
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			mknodat_verify(&test_cases[i]);
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
	tst_tmpdir();
	fs_type = tst_dev_fs_type();
	device = tst_acquire_device(cleanup);
	if (!device)
		tst_brkm(TCONF, cleanup, "Failed to acquire device");
	tst_mkfs(cleanup, device, fs_type, NULL, NULL);
	TEST_PAUSE;
	 * mount a read-only file system for EROFS test
	 */
	mkdir(cleanup, MNT_POINT, DIR_MODE);
	mount(cleanup, device, MNT_POINT, fs_type, MS_RDONLY, NULL);
	mount_flag = 1;
	dir_fd = open(cleanup, MNT_POINT, O_DIRECTORY);
	 * NOTE: the ELOOP test is written based on that the consecutive
	 * symlinks limits in kernel is hardwired to 40.
	 */
	mkdir(cleanup, "test_eloop", DIR_MODE);
	symlink(cleanup, "../test_eloop", "test_eloop/test_eloop");
	for (i = 0; i < 43; i++)
		strcat(elooppathname, ELOPFILE);
}

int  mknodat_verify(struct test_case_t *tc)
{
	int fd = *(tc->dir_fd);
	char *pathname = tc->pathname;
	mode_t mode = tc->mode;
mknodat(fd, pathname, mode, 0);
	if (TEST_ERRNO == tc->exp_errno) {
		tst_resm(TPASS | TTERRNO,
			 "mknodat() returned the expected value");
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "mknodat() got unexpected return value; expected: "
			 "%d - %s", tc->exp_errno,
			 strerror(tc->exp_errno));
	}
	if (TEST_ERRNO == 0 &&
	    tst_syscall(__NR_unlinkat, fd, pathname, 0) < 0) {
		tst_brkm(TBROK | TERRNO, cleanup, "unlinkat(%d, %s) "
			 "failed.", fd, pathname);
	}
}

void cleanup(void)
{
	if (dir_fd > 0 && close(dir_fd) < 0)
		tst_resm(TWARN | TERRNO, "close(%d) failed", dir_fd);
	if (mount_flag && tst_umount(MNT_POINT) < 0)
		tst_resm(TWARN | TERRNO, "umount device:%s failed", device);
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

