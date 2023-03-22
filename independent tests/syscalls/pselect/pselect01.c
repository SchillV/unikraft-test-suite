#include "incl.h"
int sample_fn(int clk_id, long long usec)
{
	fd_set readfds;
	struct timespec tv = tst_timespec_from_us(usec);
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	tst_timer_start(clk_id);
pselect(0, &readfds, NULL, NULL, &tv, NULL);
	tst_timer_stop();
	tst_timer_sample();
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO,
			"pselect() returned %li on timeout", TST_RET);
		return 1;
	}
	return 0;
}

void main(){
	setup();
	cleanup();
}
