#include "incl.h"
char *TCID = "mq_notify02";

void setup(void);

void cleanup(void);

struct test_case_t {
	struct sigevent sevp;
	int exp_errno;
} test_cases[] = {
	{{.sigev_notify = -1}, EINVAL},
	{{.sigev_notify = SIGEV_SIGNAL, .sigev_signo = _NSIG+1}, EINVAL},
};
int TST_TOTAL = ARRAY_SIZE(test_cases);

int  mq_notify_verify(struct test_case_t *);
int main(int argc, char **argv)
{
	int lc;
	int i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			mq_notify_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

int  mq_notify_verify(struct test_case_t *test)
{
mq_notify(0, &(test->sevp));
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL, "mq_notify() succeeded unexpectedly");
		return;
	}
	if (TEST_ERRNO == test->exp_errno) {
		tst_resm(TPASS | TTERRNO, "mq_notify failed as expected");
	} else if (TEST_ERRNO == ENOSYS) {
		tst_resm(TCONF | TTERRNO, "mq_notify not available");
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "mq_notify failed unexpectedly; expected: %d - %s",
			 test->exp_errno, strerror(test->exp_errno));
	}
}

void cleanup(void)
{
}

