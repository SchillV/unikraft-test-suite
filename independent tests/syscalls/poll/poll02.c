#include "incl.h"

int fds[2];
int sample_fn(int clk_id, long long usec)
{
	unsigned int sleep_ms = usec / 1000;
	struct pollfd pfds[] = {
		{.fd = fds[0], .events = POLLIN}
	};
	tst_timer_start(clk_id);
poll(pfds, 1, sleep_ms);
	tst_timer_stop();
	tst_timer_sample();
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "poll() returned %li", TST_RET);
		return 1;
	}
	return 0;
}

void setup(void)
{
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
