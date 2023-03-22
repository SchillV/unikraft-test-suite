#include "incl.h"
#define _GNU_SOURCE

cpu_set_t *mask, *emask, *fmask;

size_t mask_size, emask_size;

pid_t self_pid, privileged_pid, free_pid;

struct passwd *ltpuser;

struct tcase {
	pid_t *pid;
	size_t *size;
	cpu_set_t **mask;
	int exp_errno;
} tcases[] = {
	{&self_pid, &mask_size, &fmask, EFAULT},
	{&self_pid, &emask_size, &emask, EINVAL},
	{&free_pid, &mask_size, &mask, ESRCH},
	{&privileged_pid, &mask_size, &mask, EPERM},
};

void kill_pid(void)
{
	kill(privileged_pid, SIGKILL);
	waitpid(privileged_pid, NULL, 0);
	seteuid(0);
}

int  verify_test(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (tc->exp_errno == EPERM) {
		privileged_pid = fork();
		if (privileged_pid == 0) {
			pause();
			exit(0);
		}
		seteuid(ltpuser->pw_uid);
	}
tst_syscall(__NR_sched_setaffinit;
			*tc->pid, *tc->size, *tc->mask));
	if (TST_RET != -1) {
		tst_res(TFAIL, "sched_setaffinity() succeded unexpectedly");
		kill_pid();
		return;
	}
	if (TST_ERR != tc->exp_errno) {
		tst_res(TFAIL | TTERRNO,
			"sched_setaffinity() should fail with %s, got",
			tst_strerrno(tc->exp_errno));
	} else {
		tst_res(TPASS | TTERRNO, "sched_setaffinity() failed");
	}
	if (tc->exp_errno == EPERM)
		kill_pid();
}

void setup(void)
{
	long ncpus;
	ncpus = tst_ncpus_max();
	fmask = tst_get_bad_addr(NULL);
	mask = CPU_ALLOC(ncpus);
	if (!mask)
		tst_brk(TBROK | TERRNO, "CPU_ALLOC() failed");
	mask_size = CPU_ALLOC_SIZE(ncpus);
	if (sched_getaffinity(0, mask_size, mask) < 0)
		tst_brk(TBROK | TERRNO, "sched_getaffinity() failed");
	emask = CPU_ALLOC(ncpus + 1);
	if (!emask)
		tst_brk(TBROK | TERRNO, "CPU_ALLOC() failed");
	emask_size = CPU_ALLOC_SIZE(ncpus + 1);
	CPU_ZERO_S(emask_size, emask);
	CPU_SET_S(ncpus, emask_size, emask);
	ltpuser = getpwnam("nobody");
	free_pid = tst_get_unused_pid();
}

void cleanup(void)
{
	if (mask)
		CPU_FREE(mask);
	if (emask)
		CPU_FREE(emask);
}

void main(){
	setup();
	cleanup();
}
