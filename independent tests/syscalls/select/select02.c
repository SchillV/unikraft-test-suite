#include "incl.h"

int fds[2];

int sample_fn(int clk_id, long long usec)
{
	struct timeval timeout = tst_us_to_timeval(usec);
	fd_set sfds;
	FD_ZERO(&sfds);
	FD_SET(fds[0], &sfds);
	tst_timer_start(clk_id);
do_select(1, &sfds, NULL, NULL, &timeout);
	tst_timer_stop();
	tst_timer_sample();
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "select() returned %li", TST_RET);
		return 1;
	}
	return 0;
}

void setup(void)
{
	select_info();
	pipe(fds);
}

void cleanup(void)
{
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[1] > 0)
		close(fds[1]);
}

void main(){
	setup();
	cleanup();
}
