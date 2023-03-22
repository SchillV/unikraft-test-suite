#include "incl.h"
void setup();
void cleanup();
char *TCID = "munlock02";
int TST_TOTAL = 1;
#define LEN	1024
void *addr1;
struct test_case_t {
	void *addr;
	int len;
	int error;
	char *edesc;
} TC[] = {
	{
NULL, 0, ENOMEM, "address range out of address space"},};
#if !defined(UCLINUX)
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
#ifdef __ia64__
			TC[0].len = 8 * getpagesize();
#endif
munlock(TC[i].addr, TC[i].len);
			if (TEST_RETURN == -1) {
				if (TEST_ERRNO != TC[i].error)
					tst_brkm(TFAIL, cleanup,
						 "munlock() Failed with wrong "
						 "errno, expected errno=%s, "
						 "got errno=%d : %s",
						 TC[i].edesc, TEST_ERRNO,
						 strerror(TEST_ERRNO));
				else
					tst_resm(TPASS,
						 "expected failure - errno "
						 "= %d : %s",
						 TEST_ERRNO,
						 strerror(TEST_ERRNO));
			} else {
				tst_brkm(TFAIL, cleanup,
					 "munlock() Failed, expected "
					 "return value=-1, got %ld",
					 TEST_RETURN);
			}
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	char *address;
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TC[0].len = 8 * getpagesize();
	address = mmap(0, TC[0].len, PROT_READ | PROT_WRITE,
		       MAP_PRIVATE_EXCEPT_UCLINUX | MAP_ANONYMOUS, 0, 0);
	if (address == MAP_FAILED)
		tst_brkm(TFAIL, cleanup, "mmap_failed");
	memset(address, 0x20, TC[0].len);
mlock(address, TC[0].len);
	if (TEST_RETURN == -1) {
		tst_brkm(TFAIL | TTERRNO, cleanup,
			 "mlock(%p, %d) Failed with return=%ld", address,
			 TC[0].len, TEST_RETURN);
	}
	TC[0].addr = address;
	 * unmap part of the area, to create the condition for ENOMEM
	 */
	address += 2 * getpagesize();
	munmap(address, 4 * getpagesize());
	TEST_PAUSE;
	return;
}
#else
int main(void)
{
	tst_resm(TINFO, "test is not available on uClinux");
	tst_exit();
}
void cleanup(void)
{
	return;
}

