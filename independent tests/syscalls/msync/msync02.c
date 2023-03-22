#include "incl.h"
#define TEMPFILE	"msync_file"
#define BUF_SIZE	256
char *TCID = "msync02";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
msync(addr, page_sz, MS_INVALIDATE);
	if (TEST_RETURN == -1)
		tst_resm(TFAIL | TTERRNO, "msync failed");
	else if (memcmp(addr + 100, write_buf, strlen(write_buf)) != 0)
		tst_resm(TFAIL, "memory region contains invalid data");
	else
		tst_resm(TPASS, "Functionality of msync successful");
	cleanup();
	tst_exit();
}
void setup(void)
{
	char tst_buf[BUF_SIZE];
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	page_sz = (size_t)getpagesize();
	tst_tmpdir();
	if ((fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0666)) < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "open failed");
	while (c_total < page_sz) {
		if ((nwrite = write(fildes, tst_buf, sizeof(tst_buf))) <= 0)
			tst_brkm(TBROK | TERRNO, cleanup, "write failed");
		else
			c_total += nwrite;
	}
	addr = mmap(0, page_sz, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED,
		    fildes, 0);
	if (addr == MAP_FAILED)
		tst_brkm(TBROK | TERRNO, cleanup, "mmap failed");
	if (lseek(fildes, 100, SEEK_SET) != 100)
		tst_brkm(TBROK | TERRNO, cleanup, "lseek failed");
	if (write(fildes, write_buf, strlen(write_buf)) != (long)strlen(write_buf))
		tst_brkm(TBROK | TERRNO, cleanup, "write failed");
}
void cleanup(void)
{
	if (munmap(addr, page_sz) == -1)
		tst_resm(TBROK | TERRNO, "munmap failed");
	if (close(fildes) == -1)
		tst_resm(TWARN | TERRNO, "close failed");
	tst_rmdir();
}

