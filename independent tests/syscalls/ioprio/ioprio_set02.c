#include "incl.h"

void run(void)
{
	int class, prio;
	int fail_in_loop;
	class = IOPRIO_CLASS_BE;
	fail_in_loop = 0;
	for (prio = 0; prio < IOPRIO_PRIO_NUM; prio++) {
sys_ioprio_set(IOPRIO_WHO_PROCESS, ;
				    IOPRIO_PRIO_VALUE(class, prio)));
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO, "ioprio_set IOPRIO_CLASS_BE prio %d failed", prio);
			fail_in_loop = 1;
		}
	}
	if (!fail_in_loop)
		tst_res(TPASS, "tested all prios in class %s",
			 to_class_str[class]);
	class = IOPRIO_CLASS_IDLE;
	fail_in_loop = 0;
	for (prio = 0; prio < IOPRIO_PRIO_NUM; prio++) {
sys_ioprio_set(IOPRIO_WHO_PROCESS, ;
				    IOPRIO_PRIO_VALUE(class, prio)));
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO, "ioprio_set IOPRIO_CLASS_IDLE prio %d failed", prio);
			fail_in_loop = 1;
		}
	}
	if (!fail_in_loop)
		tst_res(TPASS, "tested all prios in class %s",
			 to_class_str[class]);
	class = IOPRIO_CLASS_NONE;
sys_ioprio_set(IOPRIO_WHO_PROCESS, ;
			    IOPRIO_PRIO_VALUE(class, 0)));
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "ioprio_set IOPRIO_CLASS_NONE failed");
	else
		ioprio_check_setting(class, 0, 1);
}

void main(){
	setup();
	cleanup();
}
