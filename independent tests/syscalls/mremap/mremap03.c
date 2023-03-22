#include "incl.h"
#define _GNU_SOURCE
char *TCID = "mremap03";
int TST_TOTAL = 1;

char *bad_addr;

int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Attempt to expand the existing mapped
		 * memory region (memsize) by newsize limits
		 * using mremap() should fail as specified old
		 * virtual address was not mapped.
		 */
		errno = 0;
		addr = mremap(bad_addr, memsize, newsize, MREMAP_MAYMOVE);
		TEST_ERRNO = errno;
		if (addr != MAP_FAILED) {
			tst_resm(TFAIL,
				 "mremap returned invalid value, expected: -1");
			if (munmap(addr, newsize) != 0) {
				tst_brkm(TFAIL, cleanup, "munmap fails to "
					 "unmap the expanded memory region, "
					 " error=%d", errno);
			}
			continue;
		}
		if (errno == EFAULT) {
			tst_resm(TPASS, "mremap() Fails, 'old region not "
				 "mapped', errno %d", TEST_ERRNO);
		} else {
			tst_resm(TFAIL, "mremap() Fails, "
				 "'Unexpected errno %d", TEST_ERRNO);
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	if ((page_sz = getpagesize()) < 0) {
		tst_brkm(TFAIL, NULL,
			 "getpagesize() fails to get system page size");
	}
	memsize = (1000 * page_sz);
	newsize = (memsize * 2);
	 * Set the old virtual address point to some address
	 * which is not mapped.
	 */
	bad_addr = tst_get_bad_addr(cleanup);
}
void cleanup(void)
{
}

