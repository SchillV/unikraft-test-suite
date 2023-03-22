#include "incl.h"

int my_rt_sigtimedwait(const sigset_t * set, siginfo_t * info,
			      void *timeout)
{
	return tst_syscall(__NR_rt_sigtimedwait, set, info, timeout, _NSIG/8);
}
#if (__NR_rt_sigtimedwait_time64 != __LTP__NR_INVALID_SYSCALL)

int my_rt_sigtimedwait_time64(const sigset_t * set, siginfo_t * info,
				     void *timeout)
{
	return tst_syscall(__NR_rt_sigtimedwait_time64, set, info, timeout, _NSIG/8);
}
#endif
struct sigwait_test_desc tests[] = {
	{ test_empty_set, SIGUSR1},
	{ test_unmasked_matching, SIGUSR1},
	{ test_masked_matching, SIGUSR1},
	{ test_unmasked_matching_noinfo, SIGUSR1},
	{ test_masked_matching_noinfo, SIGUSR1},
	{ test_bad_address, SIGUSR1},
	{ test_bad_address2, SIGUSR1},
	{ test_bad_address3, SIGUSR1},
	{ test_timeout, 0},
	{ test_masked_matching, SIGUSR2},
	{ test_timeout, SIGUSR2},
	{ test_masked_matching, SIGTERM},
	{ test_masked_matching_rt, -1},
};

struct time64_variants variants[] = {
#if (__NR_rt_sigtimedwait != __LTP__NR_INVALID_SYSCALL)
	{ .sigwait = my_rt_sigtimedwait, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_rt_sigtimedwait_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .sigwait = my_rt_sigtimedwait_time64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void run(unsigned int i)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct sigwait_test_desc *tc = &tests[i];
	tc->tf(tv->sigwait, tc->signo, tv->ts_type);
}

void setup(void)
{
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	sigwait_setup();
}

void main(){
	setup();
	cleanup();
}
