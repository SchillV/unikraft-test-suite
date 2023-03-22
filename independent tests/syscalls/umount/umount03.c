#include "incl.h"
#define MNTPOINT	"mntpoint"

int mount_flag;

int  verify_umount(
{
umount(MNTPOINT);
	if (TST_RET != -1) {
		tst_res(TFAIL, "umount() succeeds unexpectedly");
		return;
	}
	if (TST_ERR != EPERM) {
		tst_res(TFAIL | TTERRNO, "umount() should fail with EPERM");
		return;
	}
	tst_res(TPASS | TTERRNO, "umount() fails as expected");
}

void setup(void)
{
	struct passwd *pw;
	mkdir(MNTPOINT, 0775);
	mount(tst_device->dev, MNTPOINT, tst_device->fs_type, 0, NULL);
	mount_flag = 1;
	pw = getpwnam("nobody");
	seteuid(pw->pw_uid);
}

void cleanup(void)
{
	if (seteuid(0))
		tst_res(TWARN | TERRNO, "seteuid(0) Failed");
	if (mount_flag)
		tst_umount(MNTPOINT);
}

void main(){
	setup();
	cleanup();
}
