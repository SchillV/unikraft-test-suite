#include "incl.h"
#define SET_MODE (ADJ_OFFSET | ADJ_FREQUENCY | ADJ_MAXERROR | ADJ_ESTERROR | \
				ADJ_STATUS | ADJ_TIMECONST | ADJ_TICK)


struct timex *tim_save, *buf;

struct passwd *ltpuser;

int libc_adjtimex(struct timex *value)
{
	return adjtimex(value);
}

int sys_adjtimex(struct timex *value)
{
	return tst_syscall(__NR_adjtimex, value);
}

struct test_case {
	unsigned int modes;
	long lowlimit;
	long highlimit;
	long delta;
	int exp_err;
} tc[] = {
	{.modes = SET_MODE, .exp_err = EPERM},
	{.modes = SET_MODE, .exp_err = EFAULT},
	{.modes = ADJ_TICK, .lowlimit = 900000, .delta = 1, .exp_err = EINVAL},
	{.modes = ADJ_TICK, .highlimit = 1100000, .delta = 1, .exp_err = EINVAL},
	{.modes = ADJ_OFFSET, .highlimit = 512000L, .delta = 1, .exp_err = EINVAL},
	{.modes = ADJ_OFFSET, .lowlimit = -512000L, .delta = -1, .exp_err = EINVAL},
};

struct test_variants
{
	int (*adjtimex)(struct timex *value);
	char *desc;
} variants[] = {
	{ .adjtimex = libc_adjtimex, .desc = "libc adjtimex()"},
#if (__NR_adjtimex != __LTP__NR_INVALID_SYSCALL)
	{ .adjtimex = sys_adjtimex,  .desc = "__NR_adjtimex syscall"},
#endif
};

int  verify_adjtimex(unsigned int i)
{
	struct timex *bufp;
	struct test_variants *tv = &variants[tst_variant];
	buf->modes = tc[i].modes;
	bufp = buf;
	if (tc[i].exp_err == EPERM)
		seteuid(ltpuser->pw_uid);
	if (tc[i].exp_err == EINVAL) {
		if (tc[i].modes == ADJ_TICK) {
			if (tc[i].lowlimit)
				buf->tick = tc[i].lowlimit - tc[i].delta;
			if (tc[i].highlimit)
				buf->tick = tc[i].highlimit + tc[i].delta;
		}
		if (tc[i].modes == ADJ_OFFSET) {
			if (tc[i].lowlimit || tc[i].highlimit) {
				tst_res(TCONF, "Newer kernels normalize offset value outside range");
				return;
			}
		}
	}
	if (tc[i].exp_err == EFAULT) {
		if (tv->adjtimex != libc_adjtimex) {
			bufp = (struct timex *) -1;
		} else {
			tst_res(TCONF, "EFAULT is skipped for libc variant");
			return;
		}
	}
	TST_EXP_FAIL2(tv->adjtimex(bufp), tc[i].exp_err, "adjtimex() error");
	if (tc[i].exp_err == EPERM)
		seteuid(0);
}

void setup(void)
{
	struct test_variants *tv = &variants[tst_variant];
	size_t i;
	tst_res(TINFO, "Testing variant: %s", tv->desc);
	tim_save->modes = 0;
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
	hz = sysconf(_SC_CLK_TCK);
	for (i = 0; i < ARRAY_SIZE(tc); i++) {
		if (tc[i].modes == ADJ_TICK) {
			tc[i].highlimit /= hz;
			tc[i].lowlimit /= hz;
		}
	}
	if ((adjtimex(tim_save)) == -1) {
		tst_brk(TBROK | TERRNO,
			"adjtimex(): failed to save current params");
	}
}

void cleanup(void)
{
	tim_save->modes = SET_MODE;
	if ((adjtimex(tim_save)) == -1)
		tst_res(TWARN, "Failed to restore saved parameters");
}

void main(){
	setup();
	cleanup();
}
