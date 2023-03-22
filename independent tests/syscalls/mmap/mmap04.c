#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "mmap04";
int TST_TOTAL = 1;

size_t page_sz;

char *addr;

char *dummy;

int fildes;

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	setup();
	errno = 0;
	addr = mmap(0, page_sz, PROT_READ | PROT_EXEC, MAP_FILE | MAP_SHARED,fildes, 0);
	if (addr == MAP_FAILED) {
		printf("mmap of %s failed, error number %d\n", TEMPFILE, errno);
	}
	if (read(fildes, dummy, page_sz) < 0) {
		printf("reading %s failed\n",TEMPFILE);
		cleanup();
	}
	if (memcmp(dummy, addr, page_sz)) {
		printf("mapped memory region contains invalid data\n");
	} else {
		printf("test succeeded\n");
	}
	if (munmap(addr, page_sz) != 0) {
		printf("munmapping failed\n");
		cleanup();
	}
	cleanup();
}

void setup(void)
{
	char *tst_buff;
	page_sz = getpagesize();
	if ((tst_buff = calloc(page_sz, sizeof(char))) == NULL) {
		printf("calloc failed (tst_buff)\n");
		exit(EXIT_FAILURE);
	}
	memset(tst_buff, 'A', page_sz);
	if ((fildes = open(TEMPFILE, O_WRONLY | O_CREAT, 0666)) < 0) {
		free(tst_buff);
		printf("opening %s failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (write(fildes, tst_buff, page_sz) < (ssize_t)page_sz) {
		free(tst_buff);
		printf("writing to %s failed\n", TEMPFILE);
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
		printf("closing %s failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if ((dummy = calloc(page_sz, sizeof(char))) == NULL) {
		printf("calloc failed (dummy)\n");
		cleanup();
		exit(EXIT_FAILURE);
	}
	if ((fildes = open(TEMPFILE, O_RDONLY)) < 0) {
		printf("opening %s read-only failed", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
}

void cleanup(void)
{
	close(fildes);
	free(dummy);
}

