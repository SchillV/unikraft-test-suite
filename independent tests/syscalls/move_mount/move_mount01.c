#include "incl.h"
#define MNTPOINT	"mntpoint"
#define TCASE_ENTRY(_flags)	{.name = "Flag " #_flags, .flags = _flags}

struct tcase {
	char *name;
	unsigned int flags;
} tcases[] = {
	TCASE_ENTRY(MOVE_MOUNT_F_SYMLINKS),
	TCASE_ENTRY(MOVE_MOUNT_F_AUTOMOUNTS),
	TCASE_ENTRY(MOVE_MOUNT_F_EMPTY_PATH),
	TCASE_ENTRY(MOVE_MOUNT_T_SYMLINKS),
	TCASE_ENTRY(MOVE_MOUNT_T_AUTOMOUNTS),
	TCASE_ENTRY(MOVE_MOUNT_T_EMPTY_PATH),
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int fsmfd, fd;
fd = fsopen(tst_device->fs_type, 0);
	if (fd == -1) {
		tst_res(TFAIL | TTERRNO, "fsopen() failed");
		return;
	}
fsconfig(fd, FSCONFIG_SET_STRING, "source", tst_device->dev, 0);
	if (TST_RET == -1) {
		close(fd);
		tst_res(TFAIL | TTERRNO, "fsconfig(FSCONFIG_SET_STRING) failed");
		return;
	}
fsconfig(fd, FSCONFIG_CMD_CREATE, NULL, NULL, 0);
	if (TST_RET == -1) {
		close(fd);
		tst_res(TFAIL | TTERRNO, "fsconfig(FSCONFIG_CMD_CREATE) failed");
		return;
	}
fsmfd = fsmount(fd, 0, 0);
	close(fd);
	if (fsmfd == -1) {
		tst_res(TFAIL | TTERRNO, "fsmount() failed");
		return;
	}
move_mount(fsmfd, "", AT_FDCWD, MNTPOIN;
			tc->flags | MOVE_MOUNT_F_EMPTY_PATH));
	close(fsmfd);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "move_mount() failed");
		return;
	}
	if (tst_is_mounted_at_tmpdir(MNTPOINT)) {
		umount(MNTPOINT);
		tst_res(TPASS, "%s: move_mount() passed", tc->name);
	}
}

void main(){
	setup();
	cleanup();
}
