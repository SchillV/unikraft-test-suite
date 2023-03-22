#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "mmap05";
int TST_TOTAL = 1;

size_t page_sz;

volatile char *addr;

int fildes;

void setup(void);

void cleanup(void);

int main(int ac, char **av)
{
	char file_content;
	setup();
	errno = 0;
	addr = mmap(0, page_sz, PROT_NONE, MAP_FILE | MAP_SHARED, fildes, 0);
	if (addr == MAP_FAILED) {
		printf("mmap() failed on %s, error number %d\n", TEMPFILE, errno);
		exit(EXIT_FAILURE);
	}
	if (errno == 0) {
		printf("test succeeded\n");
	} else {
		printf("Mapped memory region with NO access is accessible\n");
		exit(EXIT_FAILURE);
	}
	if (munmap((void *)addr, page_sz) != 0) {
		printf("munmap failed, error number %d\n", errno);
		cleanup();
		exit(EXIT_FAILURE);
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
		printf("opening %s failed, eror number %d\n", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (write(fildes, tst_buff, page_sz) != (int)page_sz) {
		free(tst_buff);
		printf("writing to %s failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	free(tst_buff);
	if (fchmod(fildes, 0444) < 0) {
		printf("fchmod of %s failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (close(fildes) < 0) {
		printf("closing %s failed, error number %d", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if ((fildes = open(TEMPFILE, O_RDONLY)) < 0) {
		printf("opening %s readonly failed, error number %d\n", TEMPFILE, errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
}

void cleanup(void)
{
	close(fildes);
}

