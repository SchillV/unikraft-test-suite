#include "incl.h"
#define MNTPOINT	"mntpoint"
#define TCASE_ENTRY(_flags)	{.name = "Flag " #_flags, .flags = _flags}

struct tcase {
	char *name;
	unsigned int flags;
} tcases[] = {
	TCASE_ENTRY(0),
	TCASE_ENTRY(FSOPEN_CLOEXEC),
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int fd, fsmfd;
fd = fsopen(tst_device->fs_type, tc->flags);
	if (fd == -1) {
		tst_res(TFAIL | TTERRNO, "fsopen() failed");
		return;
	}
fsconfig(fd, FSCONFIG_SET_STRING, "source", tst_device->dev, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "fsconfig(FSCONFIG_SET_STRING) failed");
		goto out;
	}
fsconfig(fd, FSCONFIG_CMD_CREATE, NULL, NULL, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "fsconfig(FSCONFIG_CMD_CREATE) failed");
		goto out;
	}
fsmfd = fsmount(fd, 0, 0);
	if (fsmfd == -1) {
		tst_res(TFAIL | TTERRNO, "fsmount() failed");
		goto out;
	}
move_mount(fsmfd, "", AT_FDCWD, MNTPOIN;
			MOVE_MOUNT_F_EMPTY_PATH));
	close(fsmfd);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "move_mount() failed");
		goto out;
	}
	if (tst_is_mounted_at_tmpdir(MNTPOINT)) {
		umount(MNTPOINT);
		tst_res(TPASS, "%s: fsopen() passed", tc->name);
	}
out:
	close(fd);
}

void main(){
	setup();
	cleanup();
}
