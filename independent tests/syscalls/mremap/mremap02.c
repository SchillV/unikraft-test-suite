#include "incl.h"
#define _GNU_SOURCE
char *TCID = "mremap02";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Attempt to expand the existing mapped
		 * memory region (memsize) by newsize limits using
		 * mremap() should fail as old virtual address is not
		 * page aligned.
		 */
		errno = 0;
		addr = mremap(addr, memsize, newsize, MREMAP_MAYMOVE);
		TEST_ERRNO = errno;
		if (addr != MAP_FAILED) {
			tst_resm(TFAIL,
				 "mremap returned invalid value, expected: -1");
			if (munmap(addr, newsize) != 0) {
				tst_brkm(TBROK, cleanup, "munmap fails to "
					 "unmap the expanded memory region, "
					 "error=%d", errno);
			}
			continue;
		}
		if (errno == EINVAL) {
			tst_resm(TPASS, "mremap() Failed, 'invalid argument "
				 "specified' - errno %d", TEST_ERRNO);
		} else {
			tst_resm(TFAIL, "mremap() Failed, "
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
	if ((memsize = getpagesize()) < 0) {
		tst_brkm(TFAIL, NULL,
			 "getpagesize() fails to get system page size");
	}
	newsize = (memsize * 2);
	addr = (char *)(addr + (memsize - 1));
}
void cleanup(void)
{
}

