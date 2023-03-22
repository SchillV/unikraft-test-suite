#include "incl.h"
   by Masatake YAMATO <yamato@redhat.com> */
void setup();
void cleanup();
char *TCID = "gettid01";
int TST_TOTAL = 1;
pid_t my_gettid(void)
{
	return (pid_t) syscall(__NR_gettid);
}
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	 * The following loop checks looping state if -c option given
	 */
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
my_gettid();
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL, "gettid() Failed, errno=%d: %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
		} else {
			tst_resm(TPASS, "gettid() returned %ld",
				 TEST_RETURN);
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}

