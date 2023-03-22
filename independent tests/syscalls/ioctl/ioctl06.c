#include "incl.h"

int fd;

int  verify_ioctl(
{
	unsigned long ra, rab, rao;
	ioctl(fd, BLKRAGET, &rao);
	tst_res(TINFO, "BLKRAGET original value %lu", rao);
	for (ra = 0; ra <= 4096; ra += 512) {
		ioctl(fd, BLKRASET, ra);
		ioctl(fd, BLKRAGET, &rab);
		if (ra == rab)
			tst_res(TPASS, "BLKRASET %lu read back correctly", ra);
		else
			tst_res(TFAIL, "BLKRASET %lu read back %lu", ra, rab);
	}
	tst_res(TINFO, "BLKRASET restoring original value %lu", rao);
	ioctl(fd, BLKRASET, rao);
}

void setup(void)
{
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
