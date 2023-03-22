#include "incl.h"

long hz;

struct tst_timex saved, ttxc;

int supported;
struct test_case {
	unsigned int modes;
	long highlimit;
	long delta;
};
struct test_case tc[] = {
	{
	 .modes = ADJ_OFFSET_SINGLESHOT,
	},
	{
	 .modes = ADJ_OFFSET_SS_READ,
	},
	{
	 .modes = ADJ_ALL,
	},
	{
	 .modes = ADJ_OFFSET,
	 .highlimit = 500000,
	 .delta = 10000,
	},
	{
	 .modes = ADJ_FREQUENCY,
	 .delta = 100,
	},
	{
	 .modes = ADJ_MAXERROR,
	 .delta = 100,
	},
	{
	 .modes = ADJ_ESTERROR,
	 .delta = 100,
	},
	{
	 .modes = ADJ_TIMECONST,
	 .delta = 1,
	},
	{
	 .modes = ADJ_TICK,
	 .highlimit = 1100000,
	 .delta = 1000,
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
int 	struct tst_timex verify;
	long long val;
	int rval;
	memset(&ttxc, 0, sizeof(ttxc));
int 	memset(&verify, 0, sizeof(verify));
int 	ttxc.type = verify.type = tv->type;
	rval = tv->clock_adjtime(CLOCK_REALTIME, tst_timex_get(&ttxc));
	if (rval < 0) {
		tst_res(TFAIL | TERRNO, "clock_adjtime() failed %i", rval);
		return;
	}
	timex_show("GET", &ttxc);
	timex_set_field_uint(&ttxc, ADJ_MODES, tc[i].modes);
	if (tc[i].delta) {
		val = timex_get_field_long(&ttxc, tc[i].modes);
		val += tc[i].delta;
		if (tc[i].highlimit && val >= tc[i].highlimit)
			val = tc[i].highlimit;
		timex_set_field_long(&ttxc, tc[i].modes, val);
	}
	rval = tv->clock_adjtime(CLOCK_REALTIME, tst_timex_get(&ttxc));
	if (rval < 0) {
		tst_res(TFAIL | TERRNO, "clock_adjtime() failed %i", rval);
		return;
	}
	timex_show("SET", &ttxc);
int 	rval = tv->clock_adjtime(CLOCK_REALTIME, tst_timex_get(&verify));
	if (rval < 0) {
		tst_res(TFAIL | TERRNO, "clock_adjtime() failed %i", rval);
		return;
	}
int 	timex_show("VERIFY", &verify);
	if (tc[i].delta &&
	    timex_get_field_long(&ttxc, tc[i].modes) !=
int 	    timex_get_field_long(&verify, tc[i].modes)) {
		tst_res(TFAIL, "clock_adjtime(): could not set value (mode=%x)",
			tc[i].modes);
	}
	tst_res(TPASS, "clock_adjtime(): success (mode=%x)", tc[i].modes);
}

void setup(void)
{
	struct test_variants *tv = &variants[tst_variant];
	size_t i;
	int rval;
	tst_res(TINFO, "Testing variant: %s", tv->desc);
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
		if (tc[i].modes == ADJ_TICK)
			tc[i].highlimit /= hz;
		if (timex_get_field_uint(&saved, ADJ_MODES) & ADJ_NANO) {
			if (tc[i].modes == ADJ_OFFSET) {
				tc[i].highlimit *= 1000;
				tc[i].delta *= 1000;
			}
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

