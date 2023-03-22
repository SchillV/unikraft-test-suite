#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT "mnt_point"
#define TESTFILE_FLAGGED MNTPOINT"/test_file1"
#define TESTFILE_UNFLAGGED MNTPOINT"/test_file2"

int mount_flag;

const uint32_t hash_algorithms[] = {
	FS_VERITY_HASH_ALG_SHA256,
};

void test_flagged(void)
{
	struct statx buf;
	TST_EXP_PASS(statx(AT_FDCWD, TESTFILE_FLAGGED, 0, 0, &buf),
		"statx(AT_FDCWD, %s, 0, 0, &buf)", TESTFILE_FLAGGED);
	if (buf.stx_attributes & STATX_ATTR_VERITY)
		tst_res(TPASS, "STATX_ATTR_VERITY flag is set: (%"PRIu64") ",
			(uint64_t)buf.stx_attributes);
	else
		tst_res(TFAIL, "STATX_ATTR_VERITY flag is not set");
}

void test_unflagged(void)
{
	struct statx buf;
	TST_EXP_PASS(statx(AT_FDCWD, TESTFILE_UNFLAGGED, 0, 0, &buf),
		"statx(AT_FDCWD, %s, 0, 0, &buf)", TESTFILE_UNFLAGGED);
	if ((buf.stx_attributes & STATX_ATTR_VERITY) == 0)
		tst_res(TPASS, "STATX_ATTR_VERITY flag is not set");
	else
		tst_res(TFAIL, "STATX_ATTR_VERITY flag is set");
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

void flag_setup(void)
{
	int fd, attr, ret;
	struct fsverity_enable_arg enable;
	fd = open(TESTFILE_FLAGGED, O_RDONLY, 0664);
	ret = ioctl(fd, FS_IOC_GETFLAGS, &attr);
	if (ret < 0) {
		if (errno == ENOTTY)
			tst_brk(TCONF | TERRNO, "FS_IOC_GETFLAGS not supported");
		tst_brk(TBROK | TERRNO, "ioctl(%i, FS_IOC_GETFLAGS, ...)", fd);
	}
	memset(&enable, 0, sizeof(enable));
	enable.version = 1;
	enable.hash_algorithm = hash_algorithms[0];
	enable.block_size = 4096;
	enable.salt_size = 0;
	enable.salt_ptr = (intptr_t)NULL;
	enable.sig_size = 0;
	enable.sig_ptr = (intptr_t)NULL;
	ret = ioctl(fd, FS_IOC_ENABLE_VERITY, &enable);
	if (ret < 0) {
		if (errno == EOPNOTSUPP) {
			tst_brk(TCONF,
				"fs-verity is not supported on the file system or by the kernel");
		}
		tst_brk(TBROK | TERRNO, "ioctl(%i, FS_IOC_ENABLE_VERITY) failed", fd);
	}
	ret = ioctl(fd, FS_IOC_GETFLAGS, &attr);
	if ((ret == 0) && !(attr & FS_VERITY_FL))
		tst_res(TFAIL, "%i: fs-verity enabled but FS_VERITY_FL bit not set", fd);
	close(fd);
}

void setup(void)
{
mount(tst_device->dev, MNTPOINT, tst_device->fs_type, 0, NULL);
	if (TST_RET) {
		if (TST_ERR == EINVAL)
			tst_brk(TCONF, "fs-verity not supported on loopdev");
		tst_brk(TBROK | TERRNO, "mount() failed with %ld", TST_RET);
	}
	mount_flag = 1;
	fprintf(TESTFILE_FLAGGED, "a");
	fprintf(TESTFILE_UNFLAGGED, "a");
	flag_setup();
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
