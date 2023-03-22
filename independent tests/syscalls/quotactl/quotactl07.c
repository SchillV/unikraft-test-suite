#include "incl.h"
#ifdef HAVE_XFS_XQM_H
# include <xfs/xqm.h>

unsigned int invalid_type = XFS_GROUP_QUOTA << 1 | XFS_USER_QUOTA;

unsigned int valid_type = XFS_USER_QUOTA;

int mount_flag;

int  verify_quota(
{
	struct statfs before, after;
	statfs(MNTPOINT, &before);
	TST_EXP_PASS(do_quotactl(fd, QCMD(Q_XQUOTARM, USRQUOTA), tst_device->dev, 0,
			(void *)&valid_type), "do_quotactl(Q_XQUOTARM,valid_type)");
	statfs(MNTPOINT, &after);
	if (before.f_bavail <= after.f_bavail)
		tst_res(TPASS, "Q_XQUOTARM to free space, delta(%lu)", after.f_bavail - before.f_bavail);
	else
		tst_res(TFAIL, "Q_XQUOTARM to free space, delta(-%lu)", before.f_bavail - after.f_bavail);
	TST_EXP_FAIL(do_quotactl(fd, QCMD(Q_XQUOTARM, USRQUOTA), tst_device->dev, 0,
			(void *)&invalid_type), EINVAL, "do_quotactl(Q_XQUOTARM, invalid_type)");
}

void setup(void)
{
	quotactl_info();
	 * Ensure superblock has quota data, but not running. In here, we must unmount
	 * completely and mount again with '-o no quota' because 'mount -o remount, noquota'
	 * isn't sufficient to disable accounting feature.
	 */
	mount(tst_device->dev, MNTPOINT, tst_device->fs_type, 0, "usrquota");
	mount_flag = 1;
	umount(MNTPOINT);
	mount_flag = 0;
	mount(tst_device->dev, MNTPOINT, tst_device->fs_type, 0, "noquota");
	mount_flag = 1;
	fd = open(MNTPOINT, O_RDONLY);
}

void cleanup(void)
{
	if (fd > -1)
		close(fd);
	if (mount_flag && tst_umount(MNTPOINT))
		tst_res(TWARN | TERRNO, "umount(%s)", MNTPOINT);
}

void main(){
	setup();
	cleanup();
}
