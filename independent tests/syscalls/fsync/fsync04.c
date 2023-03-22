#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT	"mnt_point"
#define FNAME		MNTPOINT"/test"
#define FILE_SIZE_MB	32
#define FILE_SIZE	(FILE_SIZE_MB * TST_MB)
#define MODE		0644

int  verify_fsync(
{
	int fd;
	unsigned long written;
	fd = open(FNAME, O_RDWR|O_CREAT, MODE);
	tst_dev_sync(fd);
	tst_dev_bytes_written(tst_device->dev);
	tst_fill_fd(fd, 0, TST_MB, FILE_SIZE_MB);
fsync(fd);
	if (TST_RET)
		tst_brk(TFAIL | TTERRNO, "fsync(fd) failed");
	written = tst_dev_bytes_written(tst_device->dev);
	close(fd);
	if (written >= FILE_SIZE)
		tst_res(TPASS, "Test file synced to device");
	else
		tst_res(TFAIL, "Synced %li, expected %i", written, FILE_SIZE);
}

void main(){
	setup();
	cleanup();
}
