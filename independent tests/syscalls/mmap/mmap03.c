#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "mmap03";
int TST_TOTAL = 1;

size_t page_sz;

char *addr;

char *dummy;

int fildes;

volatile int pass = 0;

void setup(void);

void cleanup(void);

int main(int ac, char **av)
{
	setup();
	errno = 0;
	addr = mmap(0, page_sz, PROT_EXEC, MAP_FILE | MAP_SHARED, fildes, 0);
	if (addr == MAP_FAILED) {
		printf("mmap() failed on %s, error number %d\n", TEMPFILE, errno);
		exit(EXIT_FAILURE);
	}
	if (read(fildes, dummy, page_sz) < 0) {
		printf("reading %s failed, error number %d\n", TEMPFILE, errno);
		cleanup();
	}
	if (memcmp(dummy, addr, page_sz)) {
		printf("mapped memory region, contains invalid data\n");
		exit(EXIT_FAILURE);
	} else {
		printf("test succeeded\n");
	}
#if defined(__ia64__) || defined(__hppa__) || defined(__mips__)
	if (pass) {
		printf("tes succeeded\n");
	} else {
		printf("Mapped memory region with NO access is accessible\n");
		exit(EXIT_FAILURE);
	}
#endif
	if (munmap(addr, page_sz) != 0) {
		printf("failed to unmap the mmapped pages, error number %d", errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	pass = 0;
	cleanup();
}

void setup(void)
{
	char *tst_buff;
	page_sz = getpagesize();
	if ((tst_buff = calloc(page_sz, sizeof(char))) == NULL) {
		printf("calloc failed (tst_buff)");
		exit(EXIT_FAILURE);
	}
	memset(tst_buff, 'A', page_sz);
	if ((fildes = open(TEMPFILE, O_WRONLY | O_CREAT, 0666)) < 0) {
		free(tst_buff);
		printf("opening %s failed, error number %d\n",TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (write(fildes, tst_buff, page_sz) < (long)page_sz) {
		free(tst_buff);
		printf("writing to %s failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	free(tst_buff);
	if (fchmod(fildes, 0555) < 0) {
		printf("fchmod of %s failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (close(fildes) < 0) {
		printf("closing %s failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if ((dummy = calloc(page_sz, sizeof(char))) == NULL) {
		printf("calloc failed (dummy)");
		cleanup();
		exit(EXIT_FAILURE);
	}
	if ((fildes = open(TEMPFILE, O_RDONLY)) < 0) {
		printf("opening %s read-only failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
}

void cleanup(void)
{
	close(fildes);
	free(dummy);
}

