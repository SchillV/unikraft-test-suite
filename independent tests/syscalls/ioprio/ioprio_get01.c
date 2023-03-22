#include "incl.h"

void run(void)
{
	int prio, class;
sys_ioprio_get(IOPRIO_WHO_PROCESS, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "ioprio_get failed");
		return;
	}
	class = IOPRIO_PRIO_CLASS(TST_RET);
	prio = IOPRIO_PRIO_LEVEL(TST_RET);
	if (!prio_in_range(prio)) {
		tst_res(TFAIL, "ioprio out of range (%d)", prio);
		return;
	}
	if (!class_in_range(class)) {
		tst_res(TFAIL, "ioprio class of range (%d)", class);
		return;
	}
	tst_res(TPASS, "ioprio_get returned class %s prio %d",
		to_class_str[class], prio);
}

void main(){
	setup();
	cleanup();
}
