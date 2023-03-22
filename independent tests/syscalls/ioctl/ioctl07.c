#include "incl.h"

char *s_fuzz;

int fuzz = 2;

int fd;

int  verify_ioctl(
{
	int cnt, pcnt;
	ioctl(fd, RNDGETENTCNT, &cnt);
	file_scanf("/proc/sys/kernel/random/entropy_avail", "%d", &pcnt);
	tst_res(TINFO, "entropy value from ioctl: %d, proc: %d", cnt, pcnt);
	if (abs(pcnt - cnt) <= fuzz)
		tst_res(TPASS, "entropy value within expected parameters");
	else
		tst_res(TFAIL, "incorrect entropy value from ioctl");
}

void setup(void)
{
	fd = open("/dev/urandom", O_RDONLY);
	if (s_fuzz)
		fuzz = strtol(s_fuzz, 0, 4096);
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
