#include "incl.h"

struct itimerval *value, *ovalue;

int sys_setitimer(int which, void *new_value, void *old_value)
{
	return tst_syscall(__NR_setitimer, which, new_value, old_value);
}

int  verify_setitimer(unsigned int i)
{
	switch (i) {
	case 0:
		TST_EXP_FAIL(sys_setitimer(ITIMER_REAL, value, (void *)-1), EFAULT);
		break;
	case 1:
		TST_EXP_FAIL(sys_setitimer(ITIMER_VIRTUAL, value, (void *)-1), EFAULT);
		break;
	case 2:
		TST_EXP_FAIL(sys_setitimer(-ITIMER_PROF, value, ovalue), EINVAL);
		break;
	}
}

void setup(void)
{
	value->it_value.tv_sec = 30;
	value->it_value.tv_usec = 0;
	value->it_interval.tv_sec = 0;
	value->it_interval.tv_usec = 0;
}

void main(){
	setup();
	cleanup();
}
