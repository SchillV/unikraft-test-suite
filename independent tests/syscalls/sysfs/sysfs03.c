#include "incl.h"

int  verify_sysfs03(
{
	TST_EXP_POSITIVE(tst_syscall(__NR_sysfs, 3), "sysfs(3)");
}

void main(){
	setup();
	cleanup();
}
