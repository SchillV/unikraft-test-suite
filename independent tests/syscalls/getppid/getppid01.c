#include "incl.h"

int  verify_getppid(
{
	pid_t ppid, pid_max;
	file_scanf("/proc/sys/kernel/pid_max", "%d\n", &pid_max);
	ppid = getppid();
	if (ppid > pid_max)
		tst_res(TFAIL, "getppid() returned %d, out of range!", ppid);
	else
		tst_res(TPASS, "getppid() returned %d", ppid);
}

void main(){
	setup();
	cleanup();
}
