#include "incl.h"
#define INVALID_OPTION 100

int  verify_sysfs04(
{
	TST_EXP_FAIL(tst_syscall(__NR_sysfs, INVALID_OPTION),
				EINVAL, "sysfs(INVALID_OPTION)");
}

void main(){
	setup();
	cleanup();
}
