#include "incl.h"
#define _GNU_SOURCE 1
void setup(void);
void cleanup(void);
char *TCID = "getpgid02";
int TST_TOTAL = 2;
int pgid_0, pgid_1;
#define BADPID -99
struct test_case_t {
	int *id;
	int error;
} TC[] = {
	{
	&pgid_0, ESRCH},
	{
	&pgid_1, ESRCH}
};
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
getpgid(*TC[i].id);
			if (TEST_RETURN != -1) {
				tst_resm(TFAIL, "call succeeded unexpectedly");
				continue;
			}
			if (TEST_ERRNO == TC[i].error) {
				tst_resm(TPASS, "expected failure - "
					 "errno = %d : %s", TEST_ERRNO,
					 strerror(TEST_ERRNO));
			} else {
				tst_resm(TFAIL, "unexpected error - %d : %s - "
					 "expected %d", TEST_ERRNO,
					 strerror(TEST_ERRNO), TC[i].error);
			}
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	pgid_0 = BADPID;
	pgid_1 = tst_get_unused_pid(cleanup);
}
void cleanup(void)
{
}

