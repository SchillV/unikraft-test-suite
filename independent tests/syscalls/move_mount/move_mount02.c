#include "incl.h"
#define MNTPOINT	"mntpoint"
int invalid_fd = -1, fsmfd;

struct tcase {
	char *name;
	int *from_dirfd;
	const char *from_pathname;
	int to_dirfd;
	const char *to_pathname;
	unsigned int flags;
	int exp_errno;
} tcases[] = {
	{"invalid-from-fd", &invalid_fd, "", AT_FDCWD, MNTPOINT, MOVE_MOUNT_F_EMPTY_PATH, EBADF},
	{"invalid-from-path", &fsmfd, "invalid", AT_FDCWD, MNTPOINT, MOVE_MOUNT_F_EMPTY_PATH, ENOENT},
	{"invalid-to-fd", &fsmfd, "", -1, MNTPOINT, MOVE_MOUNT_F_EMPTY_PATH, EBADF},
	{"invalid-to-path", &fsmfd, "", AT_FDCWD, "invalid", MOVE_MOUNT_F_EMPTY_PATH, ENOENT},
	{"invalid-flags", &fsmfd, "", AT_FDCWD, MNTPOINT, 0x08, EINVAL},
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int fd;
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
move_mount(*tc->from_dirfd, tc->from_pathname, tc->to_dirf;
			tc->to_pathname, tc->flags));
	close(fsmfd);
	if (TST_RET != -1) {
		umount(MNTPOINT);
		tst_res(TFAIL, "%s: move_mount() succeeded unexpectedly (index: %d)",
			tc->name, n);
		return;
	}
	if (tc->exp_errno != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "%s: move_mount() should fail with %s",
			tc->name, tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "%s: move_mount() failed as expected", tc->name);
}

void main(){
	setup();
	cleanup();
}
