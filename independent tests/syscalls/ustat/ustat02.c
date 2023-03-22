#include "incl.h"
#if defined(HAVE_SYS_USTAT_H) || defined(HAVE_LINUX_TYPES_H)

dev_t invalid_dev = -1;

dev_t root_dev;
struct ustat ubuf;

struct test_case_t {
	char *err_desc;
	int exp_errno;
	char *exp_errval;
	dev_t *dev;
	struct ustat *buf;
} tc[] = {
	{"Invalid parameter", EINVAL, "EINVAL", &invalid_dev, &ubuf},
#ifndef UCLINUX
	{"Bad address", EFAULT, "EFAULT", &root_dev, (void*)-1}
#endif
};
int TST_TOTAL = ARRAY_SIZE(tc);
void run(unsigned int test)
{
tst_syscall(__NR_ustat, (unsigned int)*tc[test].dev, tc[test].buf);
	if ((TST_RET == -1) && (TST_ERR == tc[test].exp_errno))
		tst_res(TPASS | TTERRNO, "ustat(2) expected failure");
	else
		tst_res(TFAIL | TTERRNO,
			"ustat(2) failed to produce expected error; %d, errno"
			": %s", tc[test].exp_errno, tc[test].exp_errval);
}

void setup(void)
{
	struct stat buf;
	stat("/", &buf);
	root_dev = buf.st_dev;
}

void main(){
	setup();
	cleanup();
}
