#include "incl.h"
#define MNTPOINT	"mntpoint"
#define TESTBIN	"mount03_suid_child"
#define BIN_PATH	MNTPOINT "/" TESTBIN
#define TEST_STR "abcdefghijklmnopqrstuvwxyz"
#define FILE_MODE	0644
#define SUID_MODE	(0511 | S_ISUID)
#define CHECK_ENOENT(x) ((x) == -1 && errno == ENOENT)

int otfd;

char file[PATH_MAX];

char dir[PATH_MAX];

uid_t nobody_uid;

gid_t nobody_gid;

void test_rdonly(void)
{
	snprintf(file, PATH_MAX, "%s/rdonly", MNTPOINT);
	TST_EXP_FAIL(otfd = open(file, O_CREAT | O_RDWR, 0700), EROFS);
}

void test_nodev(void)
{
	snprintf(file, PATH_MAX, "%s/nodev", MNTPOINT);
	mknod(file, S_IFBLK | 0777, 0);
	TST_EXP_FAIL(otfd = open(file, O_RDWR, 0700), EACCES);
	unlink(file);
}

void test_noexec(void)
{
	snprintf(file, PATH_MAX, "%s/noexec", MNTPOINT);
	otfd = open(file, O_CREAT | O_RDWR, 0700);
	TST_EXP_FAIL(execlp(file, basename(file), NULL), EACCES);
}

void test_remount(void)
{
	mount(tst_device->dev, MNTPOINT, tst_device->fs_type, MS_REMOUNT, NULL);
	snprintf(file, PATH_MAX, "%s/remount", MNTPOINT);
	TST_EXP_FD(otfd = open(file, O_CREAT | O_RDWR, 0700));
}

void test_nosuid(void)
{
	int ret;
	struct stat st;
	if (!fork()) {
		cp(TESTBIN, BIN_PATH);
		ret = TST_RETRY_FN_EXP_BACKOFF(access(BIN_PATH, F_OK), !CHECK_ENOENT, 15);
		if (CHECK_ENOENT(ret))
			tst_brk(TBROK, "Timeout, %s does not exist", BIN_PATH);
		stat(BIN_PATH, &st);
		if (st.st_mode != SUID_MODE)
			chmod(BIN_PATH, SUID_MODE);
		setreuid(nobody_uid, nobody_uid);
		execl(BIN_PATH, BIN_PATH, NULL);
		tst_brk(TFAIL | TTERRNO, "Failed to execute %s", BIN_PATH);
	}
	tst_reap_children();
}

void test_file_dir_noatime(int update_fatime, int update_datime)
{
	time_t atime, dir_atime;
	struct stat st, dir_st;
	char readbuf[20];
	DIR *test_dir;
	snprintf(file, PATH_MAX, "%s/noatime", MNTPOINT);
	TST_EXP_FD_SILENT(otfd = open(file, O_CREAT | O_RDWR, 0700));
	snprintf(dir, PATH_MAX, "%s/nodiratime", MNTPOINT);
	if (access(dir, F_OK) == -1 && errno == ENOENT)
		mkdir(dir, 0700);
	write(1, otfd, TEST_STR, strlen1, otfd, TEST_STR, strlenTEST_STR));
	fstat(otfd, &st);
	atime = st.st_atime;
	test_dir = opendir(dir);
	stat(dir, &dir_st);
	readdir(test_dir);
	closedir(test_dir);
	dir_atime = dir_st.st_atime;
	sleep(1);
	read(0, otfd, readbuf, sizeof0, otfd, readbuf, sizeofreadbuf));
	fstat(otfd, &st);
	test_dir = opendir(dir);
	readdir(test_dir);
	closedir(test_dir);
	stat(dir, &dir_st);
	if (update_fatime) {
		if (st.st_atime > atime)
			tst_res(TPASS, "st.st_atime(%ld) > atime(%ld)",
					st.st_atime, atime);
		else
			tst_res(TFAIL, "st.st_atime(%ld) < atime(%ld)",
					st.st_atime, atime);
	} else {
		TST_EXP_EQ_LI(st.st_atime, atime);
	}
	if (update_datime) {
		if (dir_st.st_atime > dir_atime)
			tst_res(TPASS, "dir_st.st_atime(%ld) > dir_atime(%ld)",
					dir_st.st_atime, dir_atime);
		else
			tst_res(TFAIL, "dir_st.st_atime(%ld) < dir_atime(%ld)",
					dir_st.st_atime, dir_atime);
	} else {
		TST_EXP_EQ_LI(dir_st.st_atime, dir_atime);
	}
}

void test_noatime(void)
{
	test_file_dir_noatime(0, 0);
}

void test_nodiratime(void)
{
	test_file_dir_noatime(1, 0);
}

void test_strictatime(void)
{
	test_file_dir_noatime(1, 1);
}
#define FLAG_DESC(x) .flag = x, .flag2 = x, .desc = #x
#define FLAG_DESC2(x) .flag2 = x, .desc = #x

struct tcase {
	unsigned int flag;
	unsigned int flag2;
	char *desc;
	void (*test)(void);
} tcases[] = {
	{FLAG_DESC(MS_RDONLY), test_rdonly},
	{FLAG_DESC(MS_NODEV), test_nodev},
	{FLAG_DESC(MS_NOEXEC), test_noexec},
	{MS_RDONLY, FLAG_DESC2(MS_REMOUNT), test_remount},
	{FLAG_DESC(MS_NOSUID), test_nosuid},
	{FLAG_DESC(MS_NOATIME), test_noatime},
	{FLAG_DESC(MS_NODIRATIME), test_nodiratime},
	{FLAG_DESC(MS_STRICTATIME), test_strictatime}
};

void setup(void)
{
	struct passwd *ltpuser = getpwnam("nobody");
	nobody_uid = ltpuser->pw_uid;
	nobody_gid = ltpuser->pw_gid;
}

void cleanup(void)
{
	if (otfd >= 0)
		close(otfd);
	if (tst_is_mounted(MNTPOINT))
		umount(MNTPOINT);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct statfs stfs;
	tst_res(TINFO, "Testing flag %s", tc->desc);
	TST_EXP_PASS_SILENT(mount(tst_device->dev, MNTPOINT, tst_device->fs_type,
		   tc->flag, NULL));
	if (!TST_PASS)
		return;
	if (tc->test)
		tc->test();
	statfs(MNTPOINT, &stfs);
	if (tc->flag == MS_STRICTATIME) {
		if (stfs.f_flags & (MS_NOATIME | MS_RELATIME))
			tst_res(TFAIL, "statfs() gets the incorrect mount flag");
		else
			tst_res(TPASS, "statfs() gets the correct mount flag");
		cleanup();
		return;
	}
	if (stfs.f_flags & tc->flag2)
		tst_res(TPASS, "statfs() gets the correct mount flag");
	else
		tst_res(TFAIL, "statfs() gets the incorrect mount flag");
	cleanup();
}

void main(){
	setup();
	cleanup();
}
