#include "incl.h"
#define _GNU_SOURCE
char *TCID = "timerfd_create01";

struct test_case_t {
	int clockid;
	int flags;
	int exp_errno;
} test_cases[] = {
	{-1, 0, EINVAL},
	{0, -1, EINVAL},
};
int TST_TOTAL = ARRAY_SIZE(test_cases);

void setup(void);

int  timerfd_create_verify(const struct test_case_t *);

void cleanup(void);
int main(int argc, char *argv[])
{
	int lc;
	int i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			timerfd_create_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

int  timerfd_create_verify(const struct test_case_t *test)
{
timerfd_create(test->clockid, test->flags);
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL, "timerfd_create() succeeded unexpectedly");
		return;
	}
	if (TEST_ERRNO == test->exp_errno) {
		tst_resm(TPASS | TTERRNO,
			 "timerfd_create() failed as expected");
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "timerfd_create() failed unexpectedly; expected: "
			 "%d - %s", test->exp_errno, strerror(test->exp_errno));
	}
}

void cleanup(void)
{
}

