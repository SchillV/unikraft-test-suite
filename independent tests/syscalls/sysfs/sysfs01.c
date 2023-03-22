#include "incl.h"

int  verify_sysfs01(
{
	TST_EXP_POSITIVE(tst_syscall(__NR_sysfs, 1, "proc"), "sysfs(1, 'proc')");
}

void main(){
	setup();
	cleanup();
}
