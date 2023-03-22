#include "incl.h"

struct test_case {
	int option;
	char *fsname;
	int fsindex;
	char *err_desc;
	int exp_errno;
} tcases[] = {
	{1, "ext0", 0, "Invalid filesystem name", EINVAL},
	{4, NULL, 0, "Invalid option", EINVAL},
	{1, NULL, 0, "Address is out of your address space", EFAULT},
	{2, NULL, 1000, "fs_index is out of bounds", EINVAL}
};

int  verify_sysfs05(unsigned int nr)
{
	struct test_case *tc = &tcases[nr];
	char buf[1024];
	if (tc->option == 1) {
		TST_EXP_FAIL(tst_syscall(__NR_sysfs, tc->option, tc->fsname),
					tc->exp_errno, "%s", tc->err_desc);
	} else {
		TST_EXP_FAIL(tst_syscall(__NR_sysfs, tc->option, tc->fsindex, buf),
					tc->exp_errno, "%s", tc->err_desc);
	}
}

void setup(void)
{
	unsigned int i;
	char *bad_addr;
	bad_addr = tst_get_bad_addr(NULL);
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		if (tcases[i].exp_errno == EFAULT)
			tcases[i].fsname = bad_addr;
	}
}

void main(){
	setup();
	cleanup();
}
