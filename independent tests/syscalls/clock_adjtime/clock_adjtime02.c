#include "incl.h"

long hz;

struct tst_timex saved, ttxc;

int supported;

void *bad_addr;

void cleanup(void);
struct test_case {
	clockid_t clktype;
	unsigned int modes;
	long lowlimit;
	long highlimit;
	long delta;
	int exp_err;
	int droproot;
};
struct test_case tc[] = {
	{
	 .clktype = MAX_CLOCKS,
	 .exp_err = EINVAL,
	},
	{
	 .clktype = MAX_CLOCKS + 1,
	 .exp_err = EINVAL,
	},
	{
	 .clktype = CLOCK_REALTIME,
	 .modes = ADJ_ALL,
	 .exp_err = EFAULT,
	},
	{
	 .clktype = CLOCK_REALTIME,
	 .modes = ADJ_TICK,
	 .lowlimit = 900000,
	 .delta = 1,
	 .exp_err = EINVAL,
	},
	{
	 .clktype = CLOCK_REALTIME,
	 .modes = ADJ_TICK,
	 .highlimit = 1100000,
	 .delta = 1,
	 .exp_err = EINVAL,
	},
	{
	 .clktype = CLOCK_REALTIME,
	 .modes = ADJ_ALL,
	 .exp_err = EPERM,
	 .droproot = 1,
	},
};

struct test_variants {
	int (*clock_adjtime)(clockid_t clk_id, void *timex);
	enum tst_timex_type type;
	char *desc;
} variants[] = {
#if (__NR_clock_adjtime != __LTP__NR_INVALID_SYSCALL)
	{.clock_adjtime = sys_clock_adjtime, .type = TST_KERN_OLD_TIMEX, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_clock_adjtime64 != __LTP__NR_INVALID_SYSCALL)
	{.clock_adjtime = sys_clock_adjtime64, .type = TST_KERN_TIMEX, .desc = "syscall time64 with kernel spec"},
#endif
};

int  verify_clock_adjtime(unsigned int i)
{
	struct test_variants *tv = &variants[tst_variant];
	uid_t whoami = 0;
	struct tst_timex *txcptr = &ttxc;
	struct passwd *nobody;
	

const char name[] = "nobody";
	int rval;
	memset(txcptr, 0, sizeof(*txcptr));
	txcptr->type = tv->type;
	rval = tv->clock_adjtime(CLOCK_REALTIME, tst_timex_get(txcptr));
	if (rval < 0) {
		tst_res(TFAIL | TERRNO, "clock_adjtime() failed %i", rval);
		return;
	}
	timex_show("GET", txcptr);
	if (tc[i].droproot) {
		nobody = getpwnam(name);
		whoami = nobody->pw_uid;
		seteuid(whoami);
	}
	timex_set_field_uint(txcptr, ADJ_MODES, tc[i].modes);
	if (tc[i].delta) {
		if (tc[i].lowlimit)
			timex_set_field_long(&ttxc, tc[i].modes, tc[i].lowlimit - tc[i].delta);
		if (tc[i].highlimit)
			timex_set_field_long(&ttxc, tc[i].modes, tc[i].highlimit + tc[i].delta);
	}
	if (tc[i].exp_err == EFAULT) {
tv->clock_adjtime(tc[i].clktype, bad_addr);
	} else {
tv->clock_adjtime(tc[i].clktype, tst_timex_get(txcptr));
		timex_show("TEST", txcptr);
	}
	if (TST_RET >= 0) {
		tst_res(TFAIL, "clock_adjtime(): passed unexpectedly (mode=%x, "
				"uid=%d)", tc[i].modes, whoami);
		return;
	}
	if (tc[i].exp_err != TST_ERR) {
		tst_res(TFAIL | TTERRNO, "clock_adjtime(): expected %d but "
				"failed with %d (mode=%x, uid=%d)",
				tc[i].exp_err, TST_ERR, tc[i].modes, whoami);
		return;
	}
	tst_res(TPASS, "clock_adjtime(): failed as expected (mode=0x%x, "
			"uid=%d)", tc[i].modes, whoami);
	if (tc[i].droproot)
		seteuid(0);
}

void setup(void)
{
	struct test_variants *tv = &variants[tst_variant];
	size_t i;
	int rval;
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	bad_addr = tst_get_bad_addr(NULL);
	saved.type = tv->type;
	rval = tv->clock_adjtime(CLOCK_REALTIME, tst_timex_get(&saved));
	if (rval < 0) {
		tst_res(TFAIL | TERRNO, "clock_adjtime() failed %i", rval);
		return;
	}
	supported = 1;
	if (rval != TIME_OK && rval != TIME_ERROR) {
		timex_show("SAVE_STATUS", &saved);
		tst_brk(TBROK | TERRNO, "clock has on-going leap changes, "
				"returned: %i", rval);
	}
	hz = sysconf(_SC_CLK_TCK);
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		if (tc[i].modes == ADJ_TICK) {
			tc[i].highlimit /= hz;
			tc[i].lowlimit /= hz;
		}
	}
}

void cleanup(void)
{
	struct test_variants *tv = &variants[tst_variant];
	unsigned int modes = ADJ_ALL;
	int rval;
	if (supported == 0)
		return;
	if (timex_get_field_uint(&saved, ADJ_STATUS) & STA_NANO)
		modes |= ADJ_NANO;
	else
		modes |= ADJ_MICRO;
	timex_set_field_uint(&saved, ADJ_MODES, modes);
	rval = tv->clock_adjtime(CLOCK_REALTIME, tst_timex_get(&saved));
	if (rval < 0) {
		tst_res(TFAIL | TERRNO, "clock_adjtime() failed %i", rval);
		return;
	}
}

void main(){
	setup();
	cleanup();
}
