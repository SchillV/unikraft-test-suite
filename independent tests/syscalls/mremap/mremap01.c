#include "incl.h"
#define _GNU_SOURCE
#define TEMPFILE	"mremapfile"
char *TCID = "mremap01";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	tst_parse_opts(ac, av, NULL, NULL);
	tst_count = 0;
	setup();
	 * Call mremap to expand the existing mapped
	 * memory region (memsize) by newsize limits.
	 */
	addr = mremap(addr, memsize, newsize, MREMAP_MAYMOVE);
	if (addr == MAP_FAILED)
		tst_brkm(TFAIL | TERRNO, cleanup, "mremap failed");
	 * Attempt to initialize the expanded memory
	 * mapped region with data. If the map area
	 * was bad, we'd get SIGSEGV.
	 */
	for (ind = 0; ind < newsize; ind++) {
		addr[ind] = (char)ind;
	}
	 * Memory mapped area is good. Now, attempt
	 * to synchronize the mapped memory region
	 * with the file.
	 */
	if (msync(addr, newsize, MS_SYNC) != 0) {
		tst_resm(TFAIL | TERRNO, "msync failed to synch "
			 "mapped file");
	} else {
		tst_resm(TPASS, "Functionality of "
			 "mremap() is correct");
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	if ((pagesz = getpagesize()) < 0) {
		tst_brkm(TFAIL, NULL,
			 "getpagesize failed to get system page size");
	}
	memsize = (1000 * pagesz);
	newsize = (memsize * 2);
	tst_tmpdir();
	if ((fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0666)) < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "opening %s failed",
			 TEMPFILE);
	if (lseek(fildes, (off_t) memsize, SEEK_SET) != (off_t) memsize) {
		tst_brkm(TBROK | TERRNO, cleanup,
			 "lseeking to %d offset pos. failed", memsize);
	}
	if (write(fildes, "\0", 1) != 1) {
		tst_brkm(TBROK, cleanup, "writing to %s failed", TEMPFILE);
	}
	 * Call mmap to map virtual memory (memsize bytes) from the
	 * beginning of temporary file (offset is 0) into memory.
	 */
	addr = mmap(0, memsize, PROT_WRITE, MAP_SHARED, fildes, 0);
	if (addr == (char *)MAP_FAILED) {
		tst_brkm(TBROK, cleanup, "mmaping Failed on %s", TEMPFILE);
	}
	if (lseek(fildes, (off_t) newsize, SEEK_SET) != (off_t) newsize) {
		tst_brkm(TBROK, cleanup, "lseek() to %d offset pos. Failed, "
			 "error=%d : %s", newsize, errno, strerror(errno));
	}
	if (write(fildes, "\0", 1) != 1) {
		tst_brkm(TBROK | TERRNO, cleanup, "writing to %s failed",
			 TEMPFILE);
	}
}
void cleanup(void)
{
	if (munmap(addr, newsize) != 0)
		tst_brkm(TBROK | TERRNO, NULL, "munmap failed");
	close(NULL, fildes);
	tst_rmdir();
}

