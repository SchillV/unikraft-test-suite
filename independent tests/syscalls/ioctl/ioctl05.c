#include "incl.h"

int fd;

int  verify_ioctl(
{
	unsigned long size = 0;
	uint64_t size64 = 0;
	char buf;
	int ret;
	fd = open(tst_device->dev, O_RDONLY);
	ioctl(fd, BLKGETSIZE, &size);
	ioctl(fd, BLKGETSIZE64, &size64);
	if (size == size64/512) {
		tst_res(TPASS, "BLKGETSIZE returned %lu, BLKGETSIZE64 %llu",
			size, (unsigned long long)size64);
	} else {
		tst_res(TFAIL,
			"BLKGETSIZE returned %lu, BLKGETSIZE64 returned %llu",
			size, (unsigned long long)size64);
	}
	if (lseek(fd, size * 512, SEEK_SET) !=  (off_t)size * 512) {
		tst_res(TFAIL | TERRNO,
			"Cannot lseek to the end of the device");
	} else {
		tst_res(TPASS, "Could lseek to the end of the device");
	}
	ret = read(fd, &buf, 1);
	if (ret == 0) {
		tst_res(TPASS,
			"Got EOF when trying to read after the end of device");
	} else {
		tst_res(TFAIL | TERRNO,
			"Read at the end of device returned %i", ret);
	}
	close(fd);
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
