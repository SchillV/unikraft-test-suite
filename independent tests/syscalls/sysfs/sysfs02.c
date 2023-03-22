#include "incl.h"

int  verify_sysfs02(
{
	char buf[40];
	TST_EXP_PASS(tst_syscall(__NR_sysfs, 2, 0, buf), "sysfs(2,0,buf)");
}

void main(){
	setup();
	cleanup();
}
