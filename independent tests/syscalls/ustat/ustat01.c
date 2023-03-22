#include "incl.h"
#if defined(HAVE_SYS_USTAT_H) || defined(HAVE_LINUX_TYPES_H)

dev_t dev_num;
void run(void)
{
	struct ustat ubuf;
tst_syscall(__NR_ustat, (unsigned int)dev_num, &ubuf);
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "ustat(2) failed");
	else
		tst_res(TPASS, "ustat(2) passed");
}

void setup(void)
{
	struct stat buf;
	stat("/", &buf);
	dev_num = buf.st_dev;
}

void main(){
	setup();
	cleanup();
}
