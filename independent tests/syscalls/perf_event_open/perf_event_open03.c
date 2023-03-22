#include "incl.h"
#define INTEL_PT_PATH "/sys/bus/event_source/devices/intel_pt/type"
const int iterations = 12000000;

int fd = -1;

int runtime;

void setup(void)
{
	struct perf_event_attr ev = {
		.size = sizeof(struct perf_event_attr),
		.exclude_kernel = 1,
		.exclude_hv = 1,
		.exclude_idle = 1
	};
	if (access(INTEL_PT_PATH, F_OK))
		tst_brk(TCONF, "intel_pt is not available");
	file_scanf(INTEL_PT_PATH, "%d", &ev.type);
	fd = perf_event_open(&ev, getpid(), -1, -1, 0);
	runtime = tst_remaining_runtime();
}

void check_progress(int i)
{
	

float iter_per_ms;
	long long elapsed_ms;
	if (iter_per_ms)
		return;
	if (i % 1000 != 0)
		return;
	tst_timer_stop();
	elapsed_ms = tst_timer_elapsed_ms();
	if (elapsed_ms > 5000) {
		iter_per_ms = (float) i / elapsed_ms;
		tst_res(TINFO, "rate: %f iters/ms", iter_per_ms);
		tst_res(TINFO, "needed rate for current test runtime: %f iters/ms",
			(float) iterations / (runtime * 1000));
		if (iter_per_ms * 1000 * (runtime - 1) < iterations)
			tst_brk(TCONF, "System too slow to complete test in specified runtime");
	}
}

void run(void)
{
	long diff;
	int i;
	diff = read_meminfo("MemAvailable:");
	tst_timer_start(CLOCK_MONOTONIC);
	for (i = 0; i < iterations; i++) {
		ioctl(fd, PERF_EVENT_IOC_SET_FILTER, "filter,0/0@abcd");
		check_progress(i);
	}
	diff -= read_meminfo("MemAvailable:");
	if (diff > 50 * 1024)
		tst_res(TFAIL, "Likely kernel memory leak detected");
	else
		tst_res(TPASS, "No memory leak found");
}

void cleanup(void)
{
	if (fd >= 0)
		close(fd);
}

