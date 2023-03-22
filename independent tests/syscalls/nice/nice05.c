#include "incl.h"
#define _GNU_SOURCE

pthread_barrier_t barrier;

void set_nice(int nice_inc)
{
	int orig_nice;
	orig_nice = getpriority(PRIO_PROCESS, 0);
nice(nice_inc);
	if (TST_RET != (orig_nice + nice_inc)) {
		tst_brk(TBROK | TTERRNO, "nice(%d) returned %li, expected %i",
			nice_inc, TST_RET, orig_nice + nice_inc);
	}
	if (TST_ERR)
		tst_brk(TBROK | TTERRNO, "nice(%d) failed", nice_inc);
}

void do_something(void)
{
	volatile int number = 0;
	while (1) {
		number++;
pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if (TST_RET != 0) {
			tst_brk(TBROK | TRERRNO,
				"pthread_setcancelstate() failed");
		}
		pthread_testcancel();
	}
}

void *thread_fn(void *arg)
{
	set_nice((intptr_t)arg);
	pthread_barrier_wait(&barrier);
	do_something();
	return NULL;
}

void setup(void)
{
	size_t size;
	size_t i;
	int nrcpus = 1024;
	cpu_set_t *set;
	int some_cpu;
	set = CPU_ALLOC(nrcpus);
	if (!set)
		tst_brk(TBROK | TERRNO, "CPU_ALLOC()");
	size = CPU_ALLOC_SIZE(nrcpus);
	CPU_ZERO_S(size, set);
	if (sched_getaffinity(0, size, set) < 0)
		tst_brk(TBROK | TERRNO, "sched_getaffinity()");
	for (i = 0; i < size * 8; i++)
		if (CPU_ISSET_S(i, size, set))
			some_cpu = i;
	CPU_ZERO_S(size, set);
	CPU_SET_S(some_cpu, size, set);
	if (sched_setaffinity(0, size, set) < 0)
		tst_brk(TBROK | TERRNO, "sched_setaffinity()");
	CPU_FREE(set);
}

int  verify_nice(
{
	intptr_t nice_inc_high = -1;
	intptr_t nice_inc_low = -2;
	clockid_t nice_low_clockid, nice_high_clockid;
	struct timespec nice_high_ts, nice_low_ts;
	long long delta;
	pthread_t thread[2];
	pthread_barrier_init(&barrier, NULL, 3);
	pthread_create(&thread[0], NULL, thread_fn,
			(void *)nice_inc_high);
	pthread_create(&thread[1], NULL, thread_fn,
			(void *)nice_inc_low);
	pthread_barrier_wait(&barrier);
	sleep(tst_remaining_runtime());
pthread_getcpuclockid(thread[1], &nice_low_clockid);
	if (TST_RET != 0)
		tst_brk(TBROK | TRERRNO, "clock_getcpuclockid() failed");
pthread_getcpuclockid(thread[0], &nice_high_clockid);
	if (TST_RET != 0)
		tst_brk(TBROK | TRERRNO, "clock_getcpuclockid() failed");
	clock_gettime(nice_low_clockid, &nice_low_ts);
	clock_gettime(nice_high_clockid, &nice_high_ts);
	tst_res(TINFO, "Nice low thread CPU time: %ld.%09ld s",
			nice_low_ts.tv_sec, nice_low_ts.tv_nsec);
	tst_res(TINFO, "Nice high thread CPU time: %ld.%09ld s",
			nice_high_ts.tv_sec, nice_high_ts.tv_nsec);
	delta = tst_timespec_diff_ns(nice_low_ts, nice_high_ts);
	if (delta < 0) {
		tst_res(TFAIL, "executes less cycles than "
				"the high nice thread, delta: %lld ns", delta);
	} else {
		tst_res(TPASS, "executes more cycles than "
				"the high nice thread, delta: %lld ns", delta);
	}
	pthread_cancel(thread[0]);
	pthread_cancel(thread[1]);
	pthread_barrier_destroy(&barrier);
	pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);
}

