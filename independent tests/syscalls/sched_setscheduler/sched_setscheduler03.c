#include "incl.h"
#define _GNU_SOURCE
#define RLIMIT_NICE_NORMAL 20

pid_t zero_pid;

struct sched_param param[1] = { {0} };
struct test_case_t {
	pid_t *pid;
	int policy;
	struct sched_param *sched_param;
	int error;
};
struct test_case_t cases[] = {
	{
		.pid = &zero_pid,
		.policy = SCHED_OTHER,
		.sched_param = &param[0]
	},
	{
		.pid = &zero_pid,
		.policy = SCHED_BATCH,
		.sched_param = &param[0]
	},
#ifdef SCHED_IDLE
	{
		.pid = &zero_pid,
		.policy = SCHED_IDLE,
		.sched_param = &param[0]
	}
#endif
};

void l_rlimit_show(const int type, struct rlimit *limit)
{
	getrlimit(type, limit);
	tst_res(TINFO,
		"rlimit rlim_cur=%lu", (unsigned long)(limit->rlim_cur));
	tst_res(TINFO,
		"rlimit rlim_max=%lu", (unsigned long)(limit->rlim_max));
}

void l_rlimit_setup(const int type, struct rlimit *limit)
{
	struct rlimit tmp_rlimit;
	tst_res(TINFO,
		"Setting rlim_cur to %lu", (unsigned long)(limit->rlim_cur));
	tst_res(TINFO,
		"Setting rlim_max to %lu", (unsigned long)(limit->rlim_max));
	setrlimit(type, limit);
	l_rlimit_show(RLIMIT_NICE, &tmp_rlimit);
	if (tmp_rlimit.rlim_cur != limit->rlim_cur)
		tst_brk(TBROK | TERRNO, "Expect rlim_cur = %lu, get %lu",
				(unsigned long)(limit->rlim_cur),
				(unsigned long)tmp_rlimit.rlim_cur);
	if (tmp_rlimit.rlim_max != limit->rlim_max)
		tst_brk(TBROK | TERRNO, "Expect rlim_max = %lu, get %lu",
				(unsigned long)(limit->rlim_max),
				(unsigned long)(tmp_rlimit.rlim_max));
}

int  verify_fn(unsigned int i)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	tst_res(TINFO, "Verifying case[%d]: policy = %d, priority = %d",
		i + 1, cases[i].policy, cases[i].sched_param->sched_priority);
	TST_EXP_PASS(tv->sched_setscheduler(*cases[i].pid, cases[i].policy,
		     cases[i].sched_param));
}

void setup(void)
{
	struct sched_variant *tv = &sched_variants[tst_variant];
	uid_t ruid, euid, suid;
	struct rlimit limit;
	struct passwd *pw;
	uid_t nobody_uid;
	tst_res(TINFO, "Testing %s variant", tv->desc);
	pw = getpwnam("nobody");
	nobody_uid = pw->pw_uid;
	l_rlimit_show(RLIMIT_NICE, &limit);
	limit.rlim_cur = (RLIMIT_NICE_NORMAL - 1);
	limit.rlim_max = (RLIMIT_NICE_NORMAL - 1);
	l_rlimit_setup(RLIMIT_NICE, &limit);
	tst_res(TINFO, "Setting init sched policy to SCHED_OTHER");
	if (tv->sched_setscheduler(0, SCHED_OTHER, &param[0]))
		tst_brk(TBROK | TERRNO, "sched_setscheduler(0, SCHED_OTHER, 0)");
	if (tv->sched_getscheduler(0) != SCHED_OTHER)
		tst_brk(TBROK | TERRNO, "sched_getscheduler(0) != SCHED_OTHER");
	tst_res(TINFO, "Setting euid to nobody to drop privilege");
	seteuid(nobody_uid);
	getresuid(&ruid, &euid, &suid);
	if (euid != nobody_uid)
		tst_brk(TBROK | TERRNO, "ERROR seteuid(nobody_uid)");
}

void do_test(unsigned int i)
{
	int status = 0;
	pid_t f_pid = fork();
	if (f_pid == 0) {
		tst_res(TINFO, "forked pid is %d", getpid());
int 		verify_fn(i);
		exit(0);
	}
	wait(&status);
}

void main(){
	setup();
	cleanup();
}
