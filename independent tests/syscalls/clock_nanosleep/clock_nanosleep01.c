#include "incl.h"

void sighandler(int sig LTP_ATTRIBUTE_UNUSED)
{
}
enum test_type {
	NORMAL = 1,
	SEND_SIGINT = 2,
	BAD_TS_ADDR_REQ = 4,
	BAD_TS_ADDR_REM = 8,
};
#define TYPE_NAME(x) .ttype = x, .desc = #x

void *bad_addr;
struct test_case {
	long tv_sec;
	long tv_nsec;
	int exp_ret;
	int exp_err;
};

struct test_case tcase[] = {
	{
		TYPE_NAME(NORMAL),
		.clk_id = CLOCK_REALTIME,
		.flags = 0,
		.tv_sec = 0,
		.tv_nsec = -1,
		.exp_ret = -1,
		.exp_err = EINVAL,
	},
	{
		TYPE_NAME(NORMAL),
		.clk_id = CLOCK_REALTIME,
		.flags = 0,
		.tv_sec = 0,
		.tv_nsec = 1000000000,
		.exp_ret = -1,
		.exp_err = EINVAL,
	},
	{
		TYPE_NAME(NORMAL),
		.clk_id = CLOCK_THREAD_CPUTIME_ID,
		.flags = 0,
		.tv_sec = 0,
		.tv_nsec = 500000000,
		.exp_ret = -1,
		.exp_err = ENOTSUP,
	},
	{
		TYPE_NAME(SEND_SIGINT),
		.clk_id = CLOCK_REALTIME,
		.flags = 0,
		.tv_sec = 10,
		.tv_nsec = 0,
		.exp_ret = -1,
		.exp_err = EINTR,
	},
	{
		TYPE_NAME(BAD_TS_ADDR_REQ),
		.clk_id = CLOCK_REALTIME,
		.flags = 0,
		.exp_ret = -1,
		.exp_err = EFAULT,
	},
	{
		TYPE_NAME(BAD_TS_ADDR_REM),
		.clk_id = CLOCK_REALTIME,
		.flags = 0,
		.tv_sec = 10,
		.tv_nsec = 0,
		.exp_ret = -1,
		.exp_err = EFAULT,
	},
};

struct tst_ts *rq;

struct tst_ts *rm;

struct time64_variants variants[] = {
	{ .clock_nanosleep = libc_clock_nanosleep, .ts_type = TST_LIBC_TIMESPEC, .desc = "vDSO or syscall with libc spec"},
#if (__NR_clock_nanosleep != __LTP__NR_INVALID_SYSCALL)
	{ .clock_nanosleep = sys_clock_nanosleep, .ts_type = TST_KERN_OLD_TIMESPEC, .desc = "syscall with old kernel spec"},
#endif
#if (__NR_clock_nanosleep_time64 != __LTP__NR_INVALID_SYSCALL)
	{ .clock_nanosleep = sys_clock_nanosleep64, .ts_type = TST_KERN_TIMESPEC, .desc = "syscall time64 with kernel spec"},
#endif
};
void setup(void)
{
	rq->type = variants[tst_variant].ts_type;
	tst_res(TINFO, "Testing variant: %s", variants[tst_variant].desc);
	signal(SIGINT, sighandler);
	bad_addr = tst_get_bad_addr(NULL);
}

void do_test(unsigned int i)
{
	struct time64_variants *tv = &variants[tst_variant];
	struct test_case *tc = &tcase[i];
	pid_t pid = 0;
	void *request, *remain;
	memset(rm, 0, sizeof(*rm));
	rm->type = rq->type;
	tst_res(TINFO, "case %s", tc->desc);
	if (tc->ttype & (BAD_TS_ADDR_REQ | BAD_TS_ADDR_REM) &&
	    tv->clock_nanosleep == libc_clock_nanosleep) {
		tst_res(TCONF,
			"The libc wrapper may dereference req or rem");
		return;
	}
	if (tc->ttype & (SEND_SIGINT | BAD_TS_ADDR_REM))
		pid = create_sig_proc(SIGINT, 40, 500000);
	tst_ts_set_sec(rq, tc->tv_sec);
	tst_ts_set_nsec(rq, tc->tv_nsec);
	if (tc->ttype == BAD_TS_ADDR_REQ)
		request = bad_addr;
	else
		request = tst_ts_get(rq);
	if (tc->ttype == BAD_TS_ADDR_REM)
		remain = bad_addr;
	else
		remain = tst_ts_get(rm);
tv->clock_nanosleep(tc->clk_id, tc->flags, request, remain);
	if (tv->clock_nanosleep == libc_clock_nanosleep) {
		 * The return value and error number are differently set for
		 * libc syscall as compared to kernel syscall.
		 */
		if (TST_RET) {
			TST_ERR = TST_RET;
			TST_RET = -1;
		}
		 * nsleep isn't implemented by kernelf or
		 * CLOCK_THREAD_CPUTIME_ID and it returns ENOTSUP, but libc
		 * changes that error value to EINVAL.
		 */
		if (tc->clk_id == CLOCK_THREAD_CPUTIME_ID)
			tc->exp_err = EINVAL;
	}
	if (pid) {
		kill(pid, SIGTERM);
		wait(NULL);
	}
	if (tc->ttype == SEND_SIGINT) {
		long long expect_ms = tst_ts_to_ms(*rq);
		long long remain_ms = tst_ts_to_ms(*rm);
		if (tst_ts_valid(rm)) {
			tst_res(TFAIL | TTERRNO,
				"The clock_nanosleep() haven't updated"
				" timespec or it's not valid");
			return;
		}
		if (remain_ms > expect_ms) {
			tst_res(TFAIL | TTERRNO,
				"remaining time > requested time (%lld > %lld)",
				remain_ms, expect_ms);
			return;
		}
		tst_res(TPASS, "Timespec updated correctly");
	}
	if ((TST_RET != tc->exp_ret) || (TST_ERR != tc->exp_err)) {
		tst_res(TFAIL | TTERRNO, "returned %ld, expected %d,"
			" expected errno: %s (%d)", TST_RET,
			tc->exp_ret, tst_strerrno(tc->exp_err), tc->exp_err);
		return;
	}
	tst_res(TPASS | TTERRNO, "clock_nanosleep() failed with");
}

