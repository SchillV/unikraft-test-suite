#include "incl.h"
char *TCID = "signal06";
int TST_TOTAL = 5;
#if __x86_64__
#define LOOPS 30000
#define VALUE 123.456
volatile double D;
volatile int FLAGE;
char altstack[4096 * 10] __attribute__((aligned(4096)));
void test(void)
{
	int loop = 0;
	int pid = getpid();
	D = VALUE;
	while (D == VALUE && loop < LOOPS) {
		 * fp regs around c call */
		int unused;
		asm volatile ("syscall" : "=a"(unused) : "a"(__NR_tkill), "D"(pid), "S"(SIGHUP) : "rcx", "r11");
		loop++;
	}
	FLAGE = 1;
	tst_resm(TINFO, "loop = %d", loop);
	if (loop == LOOPS) {
		tst_resm(TPASS, "%s call succeeded", TCID);
	} else {
		tst_resm(TFAIL, "Bug Reproduced!");
		tst_exit();
	}
}
void sigh(int sig LTP_ATTRIBUTE_UNUSED)
{
}
void *tfunc(void *arg LTP_ATTRIBUTE_UNUSED)
{
	int i;
	for (i = -1; ; i *= -1) {
		if (i == -1) {
mprotect(altstack, sizeof(altstack), PROT_READ);
			if (TEST_RETURN == -1)
				tst_brkm(TBROK | TTERRNO, NULL, "mprotect failed");
		}
mprotect(altstack, sizeof(altstack), PROT_READ | PROT_WRITE);
		if (TEST_RETURN == -1)
			tst_brkm(TBROK | TTERRNO, NULL, "mprotect failed");
		if (FLAGE == 1)
			return NULL;
	}
}
int main(int ac, char **av)
{
	int i, lc;
	pthread_t pt;
	tst_parse_opts(ac, av, NULL, NULL);
	stack_t st = {
		.ss_sp = altstack,
		.ss_size = sizeof(altstack),
		.ss_flags = SS_ONSTACK,
	};
	struct sigaction sa = {
		.sa_handler = sigh,
	};
sigaction(SIGSEGV, &sa, NULL);
	if (TEST_RETURN == -1)
		tst_brkm(TBROK | TTERRNO, NULL,
				"SIGSEGV signal setup failed");
	sigaltstack(&st, NULL);
	sa.sa_flags = SA_ONSTACK;
sigaction(SIGHUP, &sa, NULL);
	if (TEST_RETURN == -1)
		tst_brkm(TBROK | TTERRNO, NULL,
				"SIGHUP signal setup failed");
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			FLAGE = 0;
pthread_create(&pt, NULL, tfunc, NULL);
			if (TEST_RETURN)
				tst_brkm(TBROK | TRERRNO, NULL,
						"pthread_create failed");
			test();
pthread_join(pt, NULL);
			if (TEST_RETURN)
				tst_brkm(TBROK | TRERRNO, NULL,
						"pthread_join failed");
		}
	}
	tst_exit();
}
#else
int main(void)
{
	tst_brkm(TCONF, NULL, "Only test on x86_64.");
}
#endif

