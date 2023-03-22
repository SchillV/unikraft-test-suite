#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "mmap02";
int TST_TOTAL = 1;

char *addr;

char *dummy;

size_t page_sz;

int fildes;

void setup(void);

void cleanup(void);

int main(int ac, char **av)
{
	setup();
	errno = 0;
	addr = mmap(0, page_sz, PROT_READ, MAP_FILE | MAP_SHARED, fildes, 0);
	if (addr == MAP_FAILED) {
		printf("mmap of %s failed, error number %d\n", TEMPFILE, errno);
		return 0;
	}
	if (read(fildes, dummy, page_sz) < 0) {
		printf("reading %s failed", TEMPFILE);
		cleanup();
	}
	if (memcmp(dummy, addr, page_sz)) {
			printf("mapped memory area contains invalid data\n");
	} else {
		printf("test succeeded\n");
	}
	if (munmap(addr, page_sz) != 0) {
		printf("munmapping failed, error number %d\n", errno);
	}
	cleanup();
	return 0;
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
	if ((fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0666)) < 0) {
		free(tst_buff);
		printf("opening %s failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (write(fildes, tst_buff, page_sz) < (int)page_sz) {
		free(tst_buff);
		printf("writing to %s failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	free(tst_buff);
	if (fchmod(fildes, 0444) < 0) {
		printf("fchmod(%s, 0444) failed, error number %d", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (close(fildes) < 0) {
		printf("closing %s failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if ((fildes = open(TEMPFILE, O_RDONLY)) < 0) {
		printf("reopening %s readonly failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if ((dummy = calloc(page_sz, sizeof(char))) == NULL) {
		printf("calloc failed (dummy)\n");
		cleanup();
		exit(EXIT_FAILURE);
	}
}

void cleanup(void)
{
	close(fildes);
	free(dummy);
}

