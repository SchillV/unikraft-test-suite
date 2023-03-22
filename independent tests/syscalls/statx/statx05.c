#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT "mnt_point"
#define TESTDIR_FLAGGED MNTPOINT"/test_dir1"
#define TESTDIR_UNFLAGGED MNTPOINT"/test_dir2"

int mount_flag;

void test_flagged(void)
{
	struct statx buf;
statx(AT_FDCWD, TESTDIR_FLAGGED, 0, 0, &buf);
	if (TST_RET == 0)
		tst_res(TPASS,
			"sys_statx(AT_FDCWD, %s, 0, 0, &buf)", TESTDIR_FLAGGED);
	else
		tst_brk(TFAIL | TTERRNO,
			"sys_statx(AT_FDCWD, %s, 0, 0, &buf)", TESTDIR_FLAGGED);
	if (buf.stx_attributes & STATX_ATTR_ENCRYPTED)
		tst_res(TPASS, "STATX_ATTR_ENCRYPTED flag is set");
	else
		tst_res(TFAIL, "STATX_ATTR_ENCRYPTED flag is not set");
}

void test_unflagged(void)
{
	struct statx buf;
statx(AT_FDCWD, TESTDIR_UNFLAGGED, 0, 0, &buf);
	if (TST_RET == 0)
		tst_res(TPASS,
			"sys_statx(AT_FDCWD, %s, 0, 0, &buf)",
			TESTDIR_UNFLAGGED);
	else
		tst_brk(TFAIL | TTERRNO,
			"sys_statx(AT_FDCWD, %s, 0, 0, &buf)",
			TESTDIR_UNFLAGGED);
	if ((buf.stx_attributes & STATX_ATTR_ENCRYPTED) == 0)
		tst_res(TPASS, "STATX_ATTR_ENCRYPTED flag is not set");
	else
		tst_res(TFAIL, "STATX_ATTR_ENCRYPTED flag is set");
}

struct test_cases {
	void (*tfunc)(void);
} tcases[] = {
	{&test_flagged},
	{&test_unflagged},
};

void run(unsigned int i)
{
	tcases[i].tfunc();
}

void setup(void)
{
	char opt_bsize[32];
	const char *const fs_opts[] = {"-O encrypt", opt_bsize, NULL};
	int ret;
	snprintf(opt_bsize, sizeof(opt_bsize), "-b %i", getpagesize());
	mkfs(tst_device->dev, tst_device->fs_type, fs_opts, NULL);
	mount(tst_device->dev, MNTPOINT, tst_device->fs_type, 0, 0);
	mount_flag = 1;
	mkdir(TESTDIR_FLAGGED, 0777);
	mkdir(TESTDIR_UNFLAGGED, 0777);
	ret = tst_system("echo qwery | e4crypt add_key "TESTDIR_FLAGGED);
	if (ret)
		tst_brk(TCONF, "e4crypt failed (CONFIG_EXT4_ENCRYPTION not set?)");
}

void cleanup(void)
{
	if (mount_flag)
		tst_umount(MNTPOINT);
}

void main(){
	setup();
	cleanup();
}
