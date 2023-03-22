#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <bits/cpu-set.h>
#include <sched.h>
#include <stdio.h>
#include <unistd.h>

long ncpu;

void *bad_addr;

int errno_test(pid_t pid, size_t cpusize, void *mask, int exp_errno)
{
	int ret = sched_getaffinity(pid, cpusize, mask);
	if (ret != -1) {
		printf("sched_getaffinity() returned %d, expected -1\n", ret);
		return 0;
	}
	if (errno != exp_errno) {
		printf("sched_getaffinity() should fail with %d\n", exp_errno);
		return 0;
	}
	printf("sched_getaffinity() failed as expected\n");
	return 1;
}

int do_test(void)
{
	cpu_set_t *mask;
	int nrcpus = 1024;
	unsigned len;
realloc:
	mask = CPU_ALLOC(nrcpus);
	if (!mask)
		printf("CPU_ALLOC() failed, error number %d\n", errno);
	len = CPU_ALLOC_SIZE(nrcpus);
	CPU_ZERO_S(len, mask);
	int ret = sched_getaffinity(0, len, mask);
	if (ret == -1) {
		CPU_FREE(mask);
		if (errno == EINVAL && nrcpus < (1024 << 8)) {
			nrcpus = nrcpus << 2;
			goto realloc;
		}
		printf("fail to get cpu affinity, error number %d\n", errno);
		exit(0);
	}
	long i, af_cpus = 0;
	for (i = 0; i < nrcpus; i++)
		af_cpus += !!CPU_ISSET_S(i, len, mask);
	if (af_cpus == 0) {
		printf("No cpus enabled in mask\n");
	} else if (af_cpus > ncpu) {
		printf("Enabled cpus = %li > system cpus %li\n", af_cpus, ncpu);
	} else
		printf("cpuset size = %u, enabled cpus %ld\n", len, af_cpus);
	int ok = errno_test(0, len, bad_addr, EFAULT) * errno_test(0, 0, mask, EINVAL);
	CPU_FREE(mask);
	return ok;
}

void setup(void)
{
	ncpu = sysconf(_SC_NPROCESSORS_CONF);
	printf("[I] system has %ld processor(s).\n", ncpu);
	bad_addr = NULL;
}

void main(){
	setup();
	if(do_test())
		printf("test succeeded\n");
}
