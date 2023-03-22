#include "incl.h"

void run(void)
{
	int class;
	class = IOPRIO_CLASS_BE;
	 * Test to fail with prio 8, first set prio 4 so we know what it
	 * should still be after failure, i.e. we check that the priority
	 * didn't change as a side effect of setting an invalid priority.
	 */
	sys_ioprio_set(IOPRIO_WHO_PROCESS, 0,
		       IOPRIO_PRIO_VALUE(class, 4));
sys_ioprio_set(IOPRIO_WHO_PROCESS, ;
			    IOPRIO_PRIO_VALUE(class, 8)));
	if (TST_RET == -1) {
		ioprio_check_setting(class, 4, 1);
		if (errno == EINVAL)
			tst_res(TPASS | TTERRNO, "returned correct error for wrong prio");
		else
			tst_res(TFAIL, "ioprio_set returns wrong errno %d",
				TST_ERR);
	} else {
		tst_res(TFAIL, "ioprio_set IOPRIO_CLASS_BE prio 8 should not work");
	}
	class = IOPRIO_CLASS_NONE;
sys_ioprio_set(IOPRIO_WHO_PROCESS, ;
			    IOPRIO_PRIO_VALUE(class, 4)));
	if (TST_RET == -1) {
		tst_res(TINFO, "tested illegal priority with class %s",
			to_class_str[class]);
		if (errno == EINVAL)
			tst_res(TPASS | TTERRNO, "returned correct error for wrong prio");
		else
			tst_res(TFAIL, "ioprio_set returns wrong errno %d",
				TST_ERR);
	} else {
		tst_res(TFAIL, "ioprio_set IOPRIO_CLASS_NONE should fail");
	}
}

void main(){
	setup();
	cleanup();
}
