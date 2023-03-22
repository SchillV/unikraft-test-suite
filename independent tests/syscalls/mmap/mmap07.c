#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "mmap07";
int TST_TOTAL = 1;

size_t page_sz;

char *addr;

int fildes;

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	setup();
	errno = 0;
	addr = mmap(0, page_sz, PROT_WRITE, MAP_FILE | MAP_PRIVATE, fildes, 0);
	if (addr != MAP_FAILED) {
		printf("mmap() returned invalid value, expected: %p, got %d\n",MAP_FAILED, errno);
		if (munmap(addr, page_sz) != 0) {
			printf("munmap() failed\n");
			cleanup();
		}
		exit(EXIT_FAILURE);
	}
	if (errno == EACCES) {
		printf("test succeeded\n");
	} else {
		printf("mmap failed with unexpected errno, error number %d\n", errno);
	}
	cleanup();
}

void setup(void)
{
	char *tst_buff;
	page_sz = getpagesize();
	if ((tst_buff = calloc(page_sz, sizeof(char))) == NULL) {
		printf("calloc() failed to allocate space for tst_buff\n");
		exit(EXIT_FAILURE);
	}
	memset(tst_buff, 'A', page_sz);
	if ((fildes = open(TEMPFILE, O_WRONLY | O_CREAT, 0666)) < 0) {
		free(tst_buff);
		printf("open() on %s failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (write(fildes, tst_buff, page_sz) < (int)page_sz) {
		free(tst_buff);
		printf("writing to %s failed\n", TEMPFILE);
		cleanup();
		exit(EXIT_FAILURE);
	}
	free(tst_buff);
}

void cleanup(void)
{
	close(fildes);
}

