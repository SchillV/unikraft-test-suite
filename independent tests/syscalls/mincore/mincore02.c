#include "incl.h"
char *TCID = "mincore02";
int TST_TOTAL = 1;

int fd = 0;

void *addr = NULL;

int page_size;

int num_pages = 4;

unsigned char *vec = NULL;

void cleanup(void)
{
	free(vec);
	munlock(addr, page_size * num_pages);
	munmap(addr, page_size * num_pages);
	close(fd);
	tst_rmdir();
}

void setup(void)
{
	char *buf;
	size_t size;
	tst_tmpdir();
	page_size = getpagesize();
	if (page_size == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "Unable to get page size");
	size = page_size * num_pages;
	buf = malloc(size);
	memset(buf, 42, size);
	vec = malloc((size + page_size - 1) / page_size);
	
	fd = open(cleanup, "mincore02", O_CREAT | O_RDWR,
		       S_IRUSR | S_IWUSR);
	if (write(fd, buf, size) < 0) {
		tst_brkm(TBROK | TERRNO, cleanup,
		         "Error in writing to the file");
	}
	free(buf);
	addr = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC,
	            MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		tst_brkm(TBROK | TERRNO, cleanup,
                         "Unable to map file for read/write");
	}
	if (mlock(addr, size) == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "Unable to lock the file");
}
int main(int argc, char **argv)
{
	int lock_pages, counter;
	int lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		if (mincore(addr, num_pages * page_size, vec) == -1) {
			tst_brkm(TBROK | TERRNO, cleanup,
			         "Unable to execute mincore system call");
		}
		lock_pages = 0;
		for (counter = 0; counter < num_pages; counter++) {
			if (vec[counter] & 1)
				lock_pages++;
		}
		if (lock_pages == num_pages) {
			tst_resm(TPASS, "%d pages locked, %d pages in-core", num_pages,
				 lock_pages);
		} else {
			tst_resm(TFAIL,
				 "not all locked pages are in-core: no. locked: %d, no. in-core: %d",
				 num_pages, lock_pages);
		}
	}
	cleanup();
	tst_exit();
}

