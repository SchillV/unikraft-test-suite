#include "incl.h"
#define TEMPFILE	"msync_file"
#define BUF_SIZE	256
char *TCID = "msync01";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	int nread = 0, count, err_flg = 0;
	tst_parse_opts(ac, av, NULL, NULL);
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		setup();
msync(addr, page_sz, MS_ASYNC);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL | TTERRNO, "msync failed");
			continue;
		}
		if (lseek(fildes, (off_t) 100, SEEK_SET) != 100)
			tst_brkm(TBROK | TERRNO, cleanup,
				 "lseek failed");
		 * Seeking to specified offset. successful.
		 * Now, read the data (256 bytes) and compare
		 * them with the expected.
		 */
		nread = read(fildes, read_buf, sizeof(read_buf));
		if (nread != BUF_SIZE)
			tst_brkm(TBROK, cleanup, "read failed");
		else {
			 * Check whether read data (from mapped
			 * file) contains the expected data
			 * which was initialised in the setup.
			 */
			for (count = 0; count < nread; count++)
				if (read_buf[count] != 1)
					err_flg++;
		}
		if (err_flg != 0)
			tst_resm(TFAIL,
				 "data read from file doesn't match");
		else
			tst_resm(TPASS,
				 "Functionality of msync() successful");
		cleanup();
	}
	tst_exit();
}
void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	page_sz = (size_t)getpagesize();
	tst_tmpdir();
	if ((fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0666)) < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "open failed");
	while (c_total < page_sz) {
		nwrite = write(fildes, write_buf, sizeof(write_buf));
		if (nwrite <= 0)
			tst_brkm(TBROK | TERRNO, cleanup, "write failed");
		else
			c_total += nwrite;
	}
	 * Call mmap to map virtual memory (mul. of page size bytes) from the
	 * beginning of temporary file (offset is 0) into memory.
	 */
	addr = mmap(0, page_sz, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED,
		    fildes, 0);
	if (addr == MAP_FAILED)
		tst_brkm(TBROK | TERRNO, cleanup, "mmap failed");
	memset(addr + 100, 1, 256);
}
void cleanup(void)
{
	if (munmap(addr, page_sz) == -1)
		tst_resm(TBROK | TERRNO, "munmap failed");
	if (close(fildes) == -1)
		tst_resm(TWARN | TERRNO, "close failed");
	tst_rmdir();
}

