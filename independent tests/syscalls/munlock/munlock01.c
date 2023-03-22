#include "incl.h"
void setup();
void setup1(int);
void cleanup();
char *TCID = "munlock01";
int TST_TOTAL = 4;
void *addr1;
struct test_case_t {
	void **addr;
	int len;
	void (*setupfunc) ();
} TC[] = {
	{
	&addr1, 1, setup1}, {
	&addr1, 1024, setup1}, {
	&addr1, 1024 * 1024, setup1}, {
	&addr1, 1024 * 1024 * 10, setup1}
};
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			if (TC[i].setupfunc != NULL)
				TC[i].setupfunc(i);
munlock(*(TC[i].addr), TC[i].len);
			if (TEST_RETURN == -1) {
				tst_resm(TFAIL | TTERRNO,
					 "mlock(%p, %d) Failed with "
					 "return=%ld", TC[i].addr, TC[i].len,
					 TEST_RETURN);
			} else {
				tst_resm(TPASS, "test %d passed length = %d",
					 i, TC[i].len);
			}
		}
	}
	cleanup();
	tst_exit();
}
void setup1(int i)
{
	addr1 = malloc(TC[i].len);
	if (addr1 == NULL)
		tst_brkm(TFAIL, cleanup, "malloc failed");
mlock(*(TC[i].addr), TC[i].len);
	if (TEST_RETURN == -1) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "mlock(%p, %d) Failed with return=%ld", TC[i].addr,
			 TC[i].len, TEST_RETURN);
	}
}
void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}
void cleanup(void)
{
}

