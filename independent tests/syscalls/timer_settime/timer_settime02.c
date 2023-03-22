#include "incl.h"

struct tst_its new_set, old_set;

struct tst_its *pnew_set = &new_set, *pold_set = &old_set, *null_set = NULL;

void *faulty_set;

kernel_timer_t timer;

kernel_timer_t timer_inval = (kernel_timer_t)-1;

const char * const descriptions[] = {
	"setting new_set pointer to NULL",
	"setting tv_nsec to a negative value",
	"setting tv_nsec value too high",
	"passing pointer to invalid timer_id",
	"passing invalid address for new_value",
	"passing invalid address for old_value",
};

struct testcase {
	kernel_timer_t		*timer_id;
	struct tst_its		**new_ptr;
	struct tst_its		**old_ptr;
	int			it_value_tv_nsec;
	int			error;
} tcases[] = {
	{&timer, &null_set, &pold_set, 0, EINVAL},
	{&timer, &pnew_set, &pold_set, -1, EINVAL},
	{&timer, &pnew_set, &pold_set, NSEC_PER_SEC + 1, EINVAL},
	{&timer_inval, &pnew_set, &pold_set, 0, EINVAL},
	{&timer, (struct tst_its **)&faulty_set, &pold_set, 0, EFAULT},
	{&timer, &pnew_set, (struct tst_its **)&faulty_set, 0, EFAULT},
};

struct time64_variants variants[] = {
#if (__NR_timer_settime != __LTP__NR_INVALID_SYSCALL)
	{ .timer_settime = sys_timer_settime, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_timer_settime64 != __LTP__NR_INVALID_SYSCALL)
	{ .timer_settime = sys_timer_settime64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};

void setup(void)
{
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	faulty_set = tst_get_bad_addr(NULL);
}

void run(unsigned int n)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct testcase *tc = &tcases[n];
	void *new, *old;
	unsigned int i;
	tst_res(TINFO, "Testing for %s:", descriptions[n]);
	for (i = 0; i < CLOCKS_DEFINED; ++i) {
		clock_t clock = clock_list[i];
tst_syscall(__NR_timer_create, clock, NULL, &timer);
		if (TST_RET != 0) {
			if (possibly_unsupported(clock) &&
				(TST_ERR == EINVAL || TST_ERR == ENOTSUP)) {
				tst_res(TCONF | TTERRNO, "%s unsupported",
					get_clock_str(clock));
			} else {
				tst_res(TFAIL | TTERRNO,
					"timer_create(%s) failed",
					get_clock_str(clock));
			}
			continue;
		}
		memset(&new_set, 0, sizeof(new_set));
		memset(&old_set, 0, sizeof(old_set));
		new_set.type = old_set.type = tv->ts_type;
		tst_its_set_interval_sec(&new_set, 0);
		tst_its_set_interval_nsec(&new_set, 0);
		tst_its_set_value_sec(&new_set, 5);
		tst_its_set_value_nsec(&new_set, tc->it_value_tv_nsec);
		new = (tc->new_ptr == (struct tst_its **)&faulty_set) ? faulty_set : tst_its_get(*tc->new_ptr);
		old = (tc->old_ptr == (struct tst_its **)&faulty_set) ? faulty_set : tst_its_get(*tc->old_ptr);
tv->timer_settime(*tc->timer_id, 0, new, old);
		if (tc->error != TST_ERR) {
			tst_res(TFAIL | TTERRNO,
				 "%s didn't fail as expected. Expected: %s - Got",
				 get_clock_str(clock),
				 tst_strerrno(tc->error));
		} else {
			tst_res(TPASS | TTERRNO,
				"%s failed as expected",
				get_clock_str(clock));
		}
tst_syscall(__NR_timer_delete, timer);
		if (TST_RET != 0)
			tst_res(TFAIL | TTERRNO, "timer_delete() failed!");
	}
}

void main(){
	setup();
	cleanup();
}
