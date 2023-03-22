#include "incl.h"
int sample_fn(int clk_id, long long usec)
{
	struct timespec to = tst_timespec_from_us(usec);
	futex_t futex = FUTEX_INITIALIZER;
	tst_timer_start(clk_id);
syscall(SYS_futex, &futex, FUTEX_WAIT, futex, &to, NULL, 0);
	tst_timer_stop();
	tst_timer_sample();
	if (TST_RET != -1) {
		tst_res(TFAIL, "futex_wait() returned %li, expected -1",
			TST_RET);
		return 1;
	}
	if (TST_ERR != ETIMEDOUT) {
		tst_res(TFAIL | TTERRNO, "expected errno=%s",
		        tst_strerrno(ETIMEDOUT));
		return 1;
	}
	return 0;
}

void main(){
	setup();
	cleanup();
}
