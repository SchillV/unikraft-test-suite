#include "incl.h"
#define MNTPOINT	"mntpoint"

int mount_flag;

int  verify_umount(
{
	if (mount_flag != 1) {
		mount(tst_device->dev, MNTPOINT,
			tst_device->fs_type, 0, NULL);
		mount_flag = 1;
	}
umount(MNTPOINT);
	if (TST_RET != 0 && TST_ERR == EBUSY) {
		tst_res(TINFO, "umount() Failed with EBUSY "
			"possibly some daemon (gvfsd-trash) "
			"is probing newly mounted dirs");
	}
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "umount() Failed");
		return;
	}
	tst_res(TPASS, "umount() Passed");
	mount_flag = 0;
}

void setup(void)
{
	mkdir(MNTPOINT, 0775);
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
