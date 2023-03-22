#include "incl.h"
void setup();
void setup1(int);
void cleanup();
char *TCID = "mlock01";
int TST_TOTAL = 4;
void *addr1;
struct test_case_t {
	void **addr;
	int len;
	void (*setupfunc) ();
} TC[] = {
	 * range pointed to by addr and len are not valid mapped pages
	 * in the address space of the process
	 */
	{
	&addr1, 1, setup1}, {
	&addr1, 1024, setup1}, {
	&addr1, 1024 * 1024, setup1}, {
	&addr1, 1024 * 1024 * 10, setup1}
};
#if !defined(UCLINUX)
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	 * FIXME (garrcoop): this should really test out whether or not the
	 * process's mappable address space is indeed accessible by the
	 * current user, instead of needing to be run by root all the time.
	 */
	tst_require_root();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			if (TC[i].setupfunc != NULL)
				TC[i].setupfunc(TC[i].len);
mlock(*(TC[i].addr), TC[i].len);
			 * should fail as designed, but this application
			 * */
			if (TEST_RETURN == -1)
				tst_resm(TFAIL | TTERRNO, "mlock failed");
			else
				tst_resm(TPASS, "mlock passed");
		}
	}
	cleanup();
	tst_exit();
}
#else
int main(void)
{
	tst_brkm(TCONF, NULL, "test is not available on uClinux");
}
void setup(void)
{
	TEST_PAUSE;
}
void setup1(int len)
{
	addr1 = malloc(len);
	if (addr1 == NULL)
		tst_brkm(TFAIL, cleanup, "malloc failed");
}
void cleanup(void)
{
}

