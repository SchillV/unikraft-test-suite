#include "incl.h"
#define MNTPOINT	"mntpoint"
#define TCASE_ENTRY(_flags, _attrs)	{.name = "Flag " #_flags ", Attr " #_attrs, .flags = _flags, .attrs = _attrs}

struct tcase {
	char *name;
	unsigned int flags;
	unsigned int attrs;
} tcases[] = {
	TCASE_ENTRY(0, MOUNT_ATTR_RDONLY),
	TCASE_ENTRY(0, MOUNT_ATTR_NOSUID),
	TCASE_ENTRY(0, MOUNT_ATTR_NODEV),
	TCASE_ENTRY(0, MOUNT_ATTR_NOEXEC),
	TCASE_ENTRY(0, MOUNT_ATTR_RELATIME),
	TCASE_ENTRY(0, MOUNT_ATTR_NOATIME),
	TCASE_ENTRY(0, MOUNT_ATTR_STRICTATIME),
	TCASE_ENTRY(0, MOUNT_ATTR_NODIRATIME),
	TCASE_ENTRY(FSMOUNT_CLOEXEC, MOUNT_ATTR_RDONLY),
	TCASE_ENTRY(FSMOUNT_CLOEXEC, MOUNT_ATTR_NOSUID),
	TCASE_ENTRY(FSMOUNT_CLOEXEC, MOUNT_ATTR_NODEV),
	TCASE_ENTRY(FSMOUNT_CLOEXEC, MOUNT_ATTR_NOEXEC),
	TCASE_ENTRY(FSMOUNT_CLOEXEC, MOUNT_ATTR_RELATIME),
	TCASE_ENTRY(FSMOUNT_CLOEXEC, MOUNT_ATTR_NOATIME),
	TCASE_ENTRY(FSMOUNT_CLOEXEC, MOUNT_ATTR_STRICTATIME),
	TCASE_ENTRY(FSMOUNT_CLOEXEC, MOUNT_ATTR_NODIRATIME),
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int sfd, mfd;
sfd = fsopen(tst_device->fs_type, FSOPEN_CLOEXEC);
	if (sfd == -1) {
		tst_res(TFAIL | TTERRNO, "fsopen() on %s failed",
			tst_device->fs_type);
		return;
	}
fsconfig(sfd, FSCONFIG_SET_STRING, "source", tst_device->dev, 0);
	if (TST_RET == -1) {
		close(sfd);
		tst_res(TFAIL | TTERRNO,
			"fsconfig(FSCONFIG_SET_STRING) failed to set source to %s", tst_device->dev);
		return;
	}
fsconfig(sfd, FSCONFIG_CMD_CREATE, NULL, NULL, 0);
	if (TST_RET == -1) {
		close(sfd);
		tst_res(TFAIL | TTERRNO, "fsconfig(FSCONFIG_CMD_CREATE) failed");
		return;
	}
mfd = fsmount(sfd, tc->flags, tc->attrs);
	close(sfd);
	if (mfd == -1) {
		tst_res(TFAIL | TTERRNO,
			"fsmount() failed to create a mount object");
		return;
	}
move_mount(mfd, "", AT_FDCWD, MNTPOINT, MOVE_MOUNT_F_EMPTY_PATH);
	close(mfd);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO,
			"move_mount() failed to attach to the mount point");
		return;
	}
	if (tst_is_mounted_at_tmpdir(MNTPOINT)) {
		umount(MNTPOINT);
		tst_res(TPASS, "%s: fsmount() passed", tc->name);
	}
}

void main(){
	setup();
	cleanup();
}
