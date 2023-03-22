#include "incl.h"
#define _GNU_SOURCE
char *TCID = "getrusage04";
int TST_TOTAL = 1;
#define RECORD_MAX    20
#define FACTOR_MAX    10
#ifndef RUSAGE_THREAD
#define RUSAGE_THREAD 1
#endif

long BIAS_MAX;

int opt_factor;

char *factor_str;

long factor_nr = 1;
option_t child_options[] = {
	{"m:", &opt_factor, &factor_str},
	{NULL, NULL, NULL}
};

void fusage(void);

void busyloop(long wait);

void setup(void);

void cleanup(void);
int main(int argc, char *argv[])
{
	struct rusage usage;
	unsigned long ulast, udelta, slast, sdelta;
	int i, lc;
	char msg_string[BUFSIZ];
	tst_parse_opts(argc, argv, child_options, fusage);
#if (__powerpc__) || (__powerpc64__) || (__s390__) || (__s390x__)
	tst_brkm(TCONF, NULL, "This test is not designed for current system");
#endif
	setup();
	if (opt_factor)
		factor_nr = strtol(cleanup, factor_str, 0, FACTOR_MAX);
	tst_resm(TINFO, "Using %ld as multiply factor for max [us]time "
		 "increment (1000+%ldus)!", factor_nr, BIAS_MAX * factor_nr);
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		i = 0;
		getrusage(cleanup, RUSAGE_THREAD, &usage);
		tst_resm(TINFO, "utime:%12luus; stime:%12luus",
			 usage.ru_utime.tv_usec, usage.ru_stime.tv_usec);
		ulast = usage.ru_utime.tv_usec;
		slast = usage.ru_stime.tv_usec;
		while (i < RECORD_MAX) {
			getrusage(cleanup, RUSAGE_THREAD, &usage);
			udelta = usage.ru_utime.tv_usec - ulast;
			sdelta = usage.ru_stime.tv_usec - slast;
			if (udelta > 0 || sdelta > 0) {
				i++;
				tst_resm(TINFO, "utime:%12luus; stime:%12luus",
					 usage.ru_utime.tv_usec,
					 usage.ru_stime.tv_usec);
				if ((long)udelta > 1000 + (BIAS_MAX * factor_nr)) {
					sprintf(msg_string,
						"utime increased > %ldus:",
						1000 + BIAS_MAX * factor_nr);
					tst_brkm(TFAIL, cleanup, msg_string,
						 " delta = %luus", udelta);
				}
				if ((long)sdelta > 1000 + (BIAS_MAX * factor_nr)) {
					sprintf(msg_string,
						"stime increased > %ldus:",
						1000 + BIAS_MAX * factor_nr);
					tst_brkm(TFAIL, cleanup, msg_string,
						 " delta = %luus", sdelta);
				}
			}
			ulast = usage.ru_utime.tv_usec;
			slast = usage.ru_stime.tv_usec;
			busyloop(100000);
		}
	}
	tst_resm(TPASS, "Test Passed");
	cleanup();
	tst_exit();
}

void fusage(void)
{
	printf("  -m n    use n as multiply factor for max [us]time "
	       "increment (1000+(1000*n)us),\n          default value is 1\n");
}

void busyloop(long wait)
{
	while (wait--) ;
}

unsigned long guess_timer_resolution(void)
{
	struct timespec res;
	if (clock_getres(CLOCK_REALTIME_COARSE, &res)) {
		tst_resm(TINFO,
		        "CLOCK_REALTIME_COARSE not supported, using 4000 us");
		return 4000;
	}
	if (res.tv_nsec < 1000000 || res.tv_nsec > 10000000) {
		tst_resm(TINFO, "Unexpected CLOCK_REALTIME_COARSE resolution,"
		        " using 4000 us");
		return 4000;
	}
	tst_resm(TINFO, "Expected timers granularity is %li us",
	         res.tv_nsec / 1000);
	return res.tv_nsec / 1000;
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	if (tst_is_virt(VIRT_XEN) || tst_is_virt(VIRT_KVM) || tst_is_virt(VIRT_HYPERV))
		tst_brkm(TCONF, NULL, "This testcase is not supported on this"
		        " virtual machine.");
	BIAS_MAX = guess_timer_resolution();
	TEST_PAUSE;
}

void cleanup(void)
{
}

