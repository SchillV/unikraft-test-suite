#include "incl.h"
char *TCID = "mlock04";
int TST_TOTAL = 1;
int fd, file_len = 40960;
char *testfile = "test_mlock";

void setup(void);

void cleanup(void);
int main(void)
{
	char *buf;
	int lc;
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		buf = mmap(NULL, file_len, PROT_WRITE, MAP_SHARED, fd, 0);
		if (buf == MAP_FAILED)
			tst_brkm(TBROK | TERRNO, cleanup, "mmap");
		if (mlock(buf, file_len) == -1)
			tst_brkm(TBROK | TERRNO, cleanup, "mlock");
		tst_resm(TINFO, "locked %d bytes from %p", file_len, buf);
		if (munlock(buf, file_len) == -1)
			tst_brkm(TBROK | TERRNO, cleanup, "munlock");
		munmap(cleanup, buf, file_len);
	}
	tst_resm(TPASS, "test succeeded.");
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_tmpdir();
	fd = open(cleanup, testfile, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(cleanup, fd, file_len);
	TEST_PAUSE;
}

void cleanup(void)
{
	close(fd);
	tst_rmdir();
}

