#include "incl.h"

int fd;

int  verify_ioctl(
{
	int ro = 1;
	ioctl(fd, BLKROGET, &ro);
	if (ro == 0)
		tst_res(TPASS, "BLKROGET returned 0");
	else
		tst_res(TFAIL, "BLKROGET returned %i", ro);
	ro = 1;
	ioctl(fd, BLKROSET, &ro);
	ro = 0;
	ioctl(fd, BLKROGET, &ro);
	if (ro == 0)
		tst_res(TFAIL, "BLKROGET returned 0");
	else
		tst_res(TPASS, "BLKROGET returned %i", ro);
mount(tst_device->dev, "mntpoint", tst_device->fs_type, 0, NULL);
	if (TST_RET != -1) {
		tst_res(TFAIL, "Mounting RO device RW succeeded");
		tst_umount("mntpoint");
		goto next;
	}
	if (TST_ERR == EACCES) {
		tst_res(TPASS | TTERRNO, "Mounting RO device RW failed");
		goto next;
	}
	tst_res(TFAIL | TTERRNO,
		"Mounting RO device RW failed unexpectedly expected EACCES");
next:
mount(tst_device->dev, "mntpoint", tst_device->fs_type, MS_RDONLY, NULL);
	if (TST_RET == 0) {
		tst_res(TPASS, "Mounting RO device RO works");
		tst_umount("mntpoint");
	} else {
		tst_res(TFAIL | TTERRNO, "Mounting RO device RO failed");
	}
	ro = 0;
	ioctl(fd, BLKROSET, &ro);
}

void setup(void)
{
	mkdir("mntpoint", 0777);
	fd = open(tst_device->dev, O_RDONLY);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
