#include "incl.h"
#define _GNU_SOURCE
#define MOUNT_POINT "mntpoint"
#define TESTDIR_FLAGGED MOUNT_POINT"/test_dir1"
#define TESTDIR_UNFLAGGED MOUNT_POINT"/test_dir2"

int fd, clear_flags;

int supp_compr = 1, supp_append = 1, supp_immutable = 1, supp_nodump = 1;

void run(unsigned int flag)
{
	struct statx buf;
statx(AT_FDCWD, flag ? TESTDIR_FLAGGED : TESTDIR_UNFLAGGED, 0, 0, &buf);
	if (TST_RET == 0)
		tst_res(TPASS,
			"sys_statx(AT_FDCWD, %s, 0, 0, &buf)",
			flag ? TESTDIR_FLAGGED : TESTDIR_UNFLAGGED);
	else
		tst_brk(TFAIL | TTERRNO,
			"sys_statx(AT_FDCWD, %s, 0, 0, &buf)",
			flag ? TESTDIR_FLAGGED : TESTDIR_UNFLAGGED);
	if (supp_compr) {
		if (buf.stx_attributes & STATX_ATTR_COMPRESSED)
			tst_res(flag ? TPASS : TFAIL,
				"STATX_ATTR_COMPRESSED flag is set");
		else
			tst_res(flag ? TFAIL : TPASS,
				"STATX_ATTR_COMPRESSED flag is not set");
	}
	if (supp_append) {
		if (buf.stx_attributes & STATX_ATTR_APPEND)
			tst_res(flag ? TPASS : TFAIL,
				"STATX_ATTR_APPEND flag is set");
		else
			tst_res(flag ? TFAIL : TPASS,
				"STATX_ATTR_APPEND flag is not set");
	}
	if (supp_immutable) {
		if (buf.stx_attributes & STATX_ATTR_IMMUTABLE)
			tst_res(flag ? TPASS : TFAIL,
				"STATX_ATTR_IMMUTABLE flag is set");
		else
			tst_res(flag ? TFAIL : TPASS,
				"STATX_ATTR_IMMUTABLE flag is not set");
	}
	if (supp_nodump) {
		if (buf.stx_attributes & STATX_ATTR_NODUMP)
			tst_res(flag ? TPASS : TFAIL,
				"STATX_ATTR_NODUMP flag is set");
		else
			tst_res(flag ? TFAIL : TPASS,
				"STATX_ATTR_NODUMP flag is not set");
	}
}

void caid_flags_setup(void)
{
	int attr, ret;
	fd = open(TESTDIR_FLAGGED, O_RDONLY | O_DIRECTORY);
	ret = ioctl(fd, FS_IOC_GETFLAGS, &attr);
	if (ret < 0) {
		if (errno == ENOTTY)
			tst_brk(TCONF | TERRNO, "FS_IOC_GETFLAGS not supported");
		if (!strcmp(tst_device->fs_type, "ntfs")) {
			tst_brk(TCONF | TERRNO,
				"ntfs3g does not support FS_IOC_GETFLAGS");
		}
		tst_brk(TBROK | TERRNO, "ioctl(%i, FS_IOC_GETFLAGS, ...)", fd);
	}
	if (supp_compr)
		attr |= FS_COMPR_FL;
	if (supp_append)
		attr |= FS_APPEND_FL;
	if (supp_immutable)
		attr |= FS_IMMUTABLE_FL;
	if (supp_nodump)
		attr |= FS_NODUMP_FL;
	ret = ioctl(fd, FS_IOC_SETFLAGS, &attr);
	if (ret < 0)
		tst_brk(TBROK | TERRNO, "ioctl(%i, FS_IOC_SETFLAGS, %i)", fd, attr);
	clear_flags = 1;
}

void setup(void)
{
	struct statx buf;
	mkdir(TESTDIR_FLAGGED, 0777);
	mkdir(TESTDIR_UNFLAGGED, 0777);
statx(AT_FDCWD, TESTDIR_FLAGGED, 0, 0, &buf);
	if (TST_RET == -1)
		tst_brk(TFAIL | TTERRNO,
			"sys_statx(AT_FDCWD, %s, 0, 0, &buf)", TESTDIR_FLAGGED);
	if ((buf.stx_attributes_mask & FS_COMPR_FL) == 0) {
		supp_compr = 0;
		tst_res(TCONF, "filesystem doesn't support FS_COMPR_FL");
	}
	if ((buf.stx_attributes_mask & FS_APPEND_FL) == 0) {
		supp_append = 0;
		tst_res(TCONF, "filesystem doesn't support FS_APPEND_FL");
	}
	if ((buf.stx_attributes_mask & FS_IMMUTABLE_FL) == 0) {
		supp_immutable = 0;
		tst_res(TCONF, "filesystem doesn't support FS_IMMUTABLE_FL");
	}
	if ((buf.stx_attributes_mask & FS_NODUMP_FL) == 0) {
		supp_nodump = 0;
		tst_res(TCONF, "filesystem doesn't support FS_NODUMP_FL");
	}
	if (!(supp_compr || supp_append || supp_immutable || supp_nodump))
		tst_brk(TCONF,
			"filesystem doesn't support the above any attr, skip it");
	caid_flags_setup();
}

void cleanup(void)
{
	int attr;
	if (clear_flags) {
		ioctl(fd, FS_IOC_GETFLAGS, &attr);
		attr &= ~(FS_COMPR_FL | FS_APPEND_FL | FS_IMMUTABLE_FL | FS_NODUMP_FL);
		ioctl(fd, FS_IOC_SETFLAGS, &attr);
	}
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
