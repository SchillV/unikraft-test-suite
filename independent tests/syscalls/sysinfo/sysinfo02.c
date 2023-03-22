#include "incl.h"
#define INVALID_ADDRESS ((uintptr_t)-1)
void setup();
void cleanup();
char *TCID = "sysinfo02";
int TST_TOTAL = 1;
#if !defined(UCLINUX)
int main(int ac, char **av)
{
	struct sysinfo *sysinfo_buf;
	int lc;
	sysinfo_buf = (void *)INVALID_ADDRESS;
	tst_parse_opts(ac, av, NULL, NULL);
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
sysinfo(sysinfo_buf);
		if (TEST_RETURN != 0 && TEST_ERRNO == EFAULT) {
			tst_resm(TPASS,
				 "Test to check the error code %d PASSED",
				 TEST_ERRNO);
		} else {
			tst_brkm(TFAIL, cleanup, "sysinfo() Failed, Expected -1 "
				 "returned %d/n", TEST_ERRNO);
		}
	}
	cleanup();
	tst_exit();
}
#else
int main(void)
{
	tst_resm(TINFO, "test is not available on uClinux");
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	umask(0);
	TEST_PAUSE;
}
void cleanup(void)
{
}

