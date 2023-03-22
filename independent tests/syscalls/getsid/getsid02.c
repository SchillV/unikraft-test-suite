#include "incl.h"

void run(void)
{
	pid_t unused_pid;
	unused_pid = tst_get_unused_pid();
	TST_EXP_FAIL(getsid(unused_pid), ESRCH);
}

void main(){
	setup();
	cleanup();
}
