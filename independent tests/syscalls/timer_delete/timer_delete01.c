#include "incl.h"

void run(void)
{
	unsigned int i;
	kernel_timer_t timer_id;
	for (i = 0; i < CLOCKS_DEFINED; ++i) {
		clock_t clock = clock_list[i];
		tst_res(TINFO, "Testing %s", get_clock_str(clock));
tst_syscall(__NR_timer_create, clock, NULL, &timer_id);
		if (TST_RET != 0) {
			if (possibly_unsupported(clock) &&
				(TST_ERR == EINVAL || TST_ERR == ENOTSUP)) {
				tst_res(TCONF | TTERRNO, "%s unsupported",
					get_clock_str(clock));
			} else {
				tst_res(TFAIL | TTERRNO,
					"Aborting test - timer_create(%s) failed",
					get_clock_str(clock));
			}
			continue;
		}
tst_syscall(__NR_timer_delete, timer_id);
		if (TST_RET == 0)
			tst_res(TPASS, "Timer deleted successfully!");
		else
			tst_res(TFAIL | TTERRNO, "Timer deletion failed!");
	}
}

void main(){
	setup();
	cleanup();
}
