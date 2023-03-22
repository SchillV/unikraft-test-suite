#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT        "mntpoint"
#define OT_MNTPOINT     "ot_mntpoint"
#define TCASE_ENTRY(attrs, exp_attrs)   \
	{                                \
		.name = #attrs,                 \
		.mount_attrs = attrs,           \
		.expect_attrs = exp_attrs       \
	}

int mount_flag, otfd = -1;

struct tcase {
	char *name;
	unsigned int mount_attrs;
	unsigned int expect_attrs;
} tcases[] = {
	TCASE_ENTRY(MOUNT_ATTR_RDONLY, ST_RDONLY),
	TCASE_ENTRY(MOUNT_ATTR_NOSUID, ST_NOSUID),
	TCASE_ENTRY(MOUNT_ATTR_NODEV, ST_NODEV),
	TCASE_ENTRY(MOUNT_ATTR_NOEXEC, ST_NOEXEC),
	TCASE_ENTRY(MOUNT_ATTR_NOSYMFOLLOW, ST_NOSYMFOLLOW),
	TCASE_ENTRY(MOUNT_ATTR_NODIRATIME, ST_NODIRATIME),
};

void cleanup(void)
{
	if (otfd > -1)
		close(otfd);
	if (mount_flag)
		umount(OT_MNTPOINT);
}

void setup(void)
{
	fsopen_supported_by_kernel();
	struct stat st = {0};
	if (stat(OT_MNTPOINT, &st) == -1)
		mkdir(OT_MNTPOINT, 0777);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct mount_attr attr = {
		.attr_set = tc->mount_attrs,
	};
	struct statvfs buf;
	TST_EXP_FD_SILENT(open_tree(AT_FDCWD, MNTPOINT, AT_EMPTY_PATH |
		AT_SYMLINK_NOFOLLOW | OPEN_TREE_CLOEXEC | OPEN_TREE_CLONE));
	if (!TST_PASS)
		return;
	otfd = (int)TST_RET;
	TST_EXP_PASS_SILENT(mount_setattr(otfd, "", AT_EMPTY_PATH, &attr, sizeof(attr)),
		"%s set", tc->name);
	if (!TST_PASS)
		goto out1;
	TST_EXP_PASS_SILENT(move_mount(otfd, "", AT_FDCWD, OT_MNTPOINT, MOVE_MOUNT_F_EMPTY_PATH));
	if (!TST_PASS)
		goto out1;
	mount_flag = 1;
	close(otfd);
	TST_EXP_PASS_SILENT(statvfs(OT_MNTPOINT, &buf), "statvfs sucess");
	if (!TST_PASS)
		goto out2;
	if (buf.f_flag & tc->expect_attrs)
		tst_res(TPASS, "%s is actually set as expected", tc->name);
	else
		tst_res(TFAIL, "%s is not actually set as expected", tc->name);
	goto out2;
out1:
	close(otfd);
out2:
	if (mount_flag)
		umount(OT_MNTPOINT);
	mount_flag = 0;
}

void main(){
	setup();
	cleanup();
}
