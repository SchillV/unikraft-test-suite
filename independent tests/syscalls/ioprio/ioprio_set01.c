#include "incl.h"

int orig_class;

int orig_prio;

void run(void)
{
	int class = orig_class, prio = orig_prio;
	class = IOPRIO_CLASS_BE;
	prio = prio + 1;
	if (!prio_in_range(prio)) {
		tst_res(TCONF, "ioprio increase out of range (%d)", prio);
		goto second;
	}
sys_ioprio_set(IOPRIO_WHO_PROCESS, ;
			    IOPRIO_PRIO_VALUE(class, prio)));
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "ioprio_set failed");
	else
		ioprio_check_setting(class, prio, 1);
second:
	prio = prio - 2;
	if (!prio_in_range(prio)) {
		tst_res(TCONF, "ioprio decrease out of range (%d)", prio);
		return;
	}
sys_ioprio_set(IOPRIO_WHO_PROCESS, ;
			    IOPRIO_PRIO_VALUE(class, prio)));
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "ioprio_set failed");
	else
		ioprio_check_setting(class, prio, 1);
}

void setup(void)
{
sys_ioprio_get(IOPRIO_WHO_PROCESS, 0);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "ioprio_get failed");
	orig_class = IOPRIO_PRIO_CLASS(TST_RET);
	orig_prio = IOPRIO_PRIO_LEVEL(TST_RET);
	tst_res(TINFO, "ioprio_get returned class %s prio %d",
		to_class_str[orig_class], orig_prio);
}

void main(){
	setup();
	cleanup();
}
