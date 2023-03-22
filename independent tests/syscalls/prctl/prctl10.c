#include "incl.h"
#define TCASE_ENTRY(tsc_read_stat) { .name = #tsc_read_stat, .read_stat = tsc_read_stat}

const char * const tsc_read_stat_names[] = {
	[0] = "[not set]",
	[PR_TSC_ENABLE] = "PR_TSC_ENABLE",
	[PR_TSC_SIGSEGV] = "PR_TSC_SIGSEGV",
};

struct tcase {
	char *name;
	int read_stat;
} tcases[] = {
	TCASE_ENTRY(PR_TSC_ENABLE),
	TCASE_ENTRY(PR_TSC_SIGSEGV)
};

uint64_t rdtsc(void)
{
	uint32_t lo = 0, hi = 0;
#if (defined(__x86_64__) || defined(__i386__))
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
#endif
	return (uint64_t)hi << 32 | lo;
}

int expected_status(int status, int exp_status)
{
	if (!exp_status && WIFEXITED(status))
		return 0;
	if (exp_status && WIFSIGNALED(status) && WTERMSIG(status) == exp_status)
		return 0;
	return 1;
}

int verify_prctl(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	unsigned long long time1, time2;
	int tsc_val = 0, pid, status, ret, ret2;
	pid = fork();
	if (!pid) {
		prctl(PR_SET_TSC, tc->read_stat);
		prctl(PR_GET_TSC, &tsc_val);
		if (tsc_val == tc->read_stat){
			printf("current state is %s(%d)\n", tc->name, tc->read_stat);
		} else{
			printf("current state is %s(%d), expect %s(%d)",tsc_read_stat_names[tsc_val], tsc_val, tc->name, tc->read_stat);
			return 0;
		}
		time1 = rdtsc();
		time2 = rdtsc();
		if (time2 > time1){
			printf("rdtsc works correctly, %lld ->%lld\n", time1, time2);
		}else
			printf("rdtsc works incorrectly, %lld ->%lld\n", time1, time2);
		return 0;
	}
	waitpid(pid, &status, 0);
	if (expected_status(status, tc->read_stat == PR_TSC_SIGSEGV ? SIGSEGV : 0)){
		printf("Test %s failed\n", tc->name);
		return 0;
	} else {
		printf("Test %s succeeded\n", tc->name);
		return 1;
	}
}

void main(){
	int ok=1;
	for(int i = 0;i<2;i++)
		ok *= verify_prctl(i);
	if(ok)
		printf("test succeeded\n");
}
