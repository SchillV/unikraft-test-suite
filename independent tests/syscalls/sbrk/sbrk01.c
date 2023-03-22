#include "incl.h"
char *TCID = "sbrk01";

struct test_case_t {
	long increment;
} test_cases[] = {
	{8192},
	{-8192},
};

void setup(void);

int  sbrk_verify(const struct test_case_t *);

void cleanup(void);
int TST_TOTAL = ARRAY_SIZE(test_cases);
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			sbrk_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

int  sbrk_verify(const struct test_case_t *test)
{
	void *tret;
	tret = sbrk(test->increment);
	TEST_ERRNO = errno;
	if (tret == (void *)-1) {
		tst_resm(TFAIL | TTERRNO, "sbrk - Increase by %ld bytes failed",
			 test->increment);
	} else {
		tst_resm(TPASS, "sbrk - Increase by %ld bytes returned %p",
			 test->increment, tret);
	}
}

void cleanup(void)
{
}

