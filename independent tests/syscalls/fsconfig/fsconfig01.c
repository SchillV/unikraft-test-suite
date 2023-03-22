#include "incl.h"
#define MNTPOINT	"mntpoint"

int fd;

void cleanup(void)
{
	if (fd != -1)
		close(fd);
}

void run(void)
{
	int fsmfd;
fd = fsopen(tst_device->fs_type, 0);
	if (fd == -1)
		tst_brk(TBROK | TTERRNO, "fsopen() failed");
fsconfig(fd, FSCONFIG_SET_FLAG, "rw", NULL, 0);
	if (TST_RET == -1)
		tst_brk(TFAIL | TTERRNO, "fsconfig(FSCONFIG_SET_FLAG) failed");
fsconfig(fd, FSCONFIG_SET_STRING, "source", tst_device->dev, 0);
	if (TST_RET == -1)
		tst_brk(TFAIL | TTERRNO, "fsconfig(FSCONFIG_SET_STRING) failed");
fsconfig(fd, FSCONFIG_SET_PATH, "sync", tst_device->dev, 0);
	if (TST_RET == -1) {
		if (TST_ERR == EOPNOTSUPP)
			tst_res(TCONF, "fsconfig(FSCONFIG_SET_PATH) not supported");
		else
			tst_brk(TFAIL | TTERRNO, "fsconfig(FSCONFIG_SET_PATH) failed");
	}
fsconfig(fd, FSCONFIG_SET_PATH_EMPTY, "sync", tst_device->dev, 0);
	if (TST_RET == -1) {
		if (TST_ERR == EOPNOTSUPP)
			tst_res(TCONF, "fsconfig(FSCONFIG_SET_PATH_EMPTY) not supported");
		else
			tst_brk(TFAIL | TTERRNO, "fsconfig(FSCONFIG_SET_PATH_EMPTY) failed");
	}
fsconfig(fd, FSCONFIG_SET_FD, "sync", NULL, 0);
	if (TST_RET == -1) {
		if (TST_ERR == EOPNOTSUPP)
			tst_res(TCONF, "fsconfig(FSCONFIG_SET_FD) not supported");
		else
			tst_brk(TFAIL | TTERRNO, "fsconfig(FSCONFIG_SET_FD) failed");
	}
fsconfig(fd, FSCONFIG_CMD_CREATE, NULL, NULL, 0);
	if (TST_RET == -1)
		tst_brk(TFAIL | TTERRNO, "fsconfig(FSCONFIG_CMD_CREATE) failed");
fsmfd = fsmount(fd, 0, 0);
	if (fsmfd == -1)
		tst_brk(TBROK | TTERRNO, "fsmount() failed");
move_mount(fsmfd, "", AT_FDCWD, MNTPOIN;
			MOVE_MOUNT_F_EMPTY_PATH));
	close(fsmfd);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "move_mount() failed");
	if (tst_is_mounted_at_tmpdir(MNTPOINT)) {
		umount(MNTPOINT);
		tst_res(TPASS, "fsconfig() passed");
	}
	close(fd);
}

void main(){
	setup();
	cleanup();
}
