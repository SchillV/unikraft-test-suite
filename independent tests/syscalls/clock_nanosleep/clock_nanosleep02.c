#include "incl.h"
int sample_fn(int clk_id, long long usec)
{
	struct timespec t = tst_timespec_from_us(usec);
	tst_timer_start(clk_id);
clock_nanosleep(clk_id, 0, &t, NULL);
	tst_timer_stop();
	tst_timer_sample();
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO,
			"nanosleep() returned %li", TST_RET);
		return 1;
	}
	return 0;
}

void main(){
	setup();
	cleanup();
}
