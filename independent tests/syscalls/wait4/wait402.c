#include "incl.h"
  * wait402 - check for ECHILD errno when using an illegal pid value
  */
#define _USE_BSD
char *TCID = "wait402";
int TST_TOTAL = 1;

void cleanup(void);

void setup(void);

long get_pid_max(void)
{
	long pid_max;
	file_scanf(NULL, "/proc/sys/kernel/pid_max", "%ld", &pid_max);
	return pid_max;
}
int main(int ac, char **av)
{
	int lc;
	pid_t epid = get_pid_max() + 1;
	int status = 1;
	struct rusage rusage;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
wait4(epid, &status, 0, &rusage);
		if (TEST_RETURN == 0) {
			tst_brkm(TFAIL, cleanup,
				 "call failed to produce expected error - errno = %d - %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
		}
		switch (TEST_ERRNO) {
		case ECHILD:
			tst_resm(TPASS,
				 "received expected failure - errno = %d - %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
			break;
		default:
			tst_brkm(TFAIL, cleanup,
				 "call failed to produce expected "
				 "error - errno = %d - %s", TEST_ERRNO,
				 strerror(TEST_ERRNO));
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
}

