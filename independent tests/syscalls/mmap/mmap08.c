#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "mmap08";
int TST_TOTAL = 1;

size_t page_sz;

char *addr;

int fildes;

void setup(void);

int main(int ac, char **av)
{
	setup();
	errno = 0;
	addr = mmap(0, page_sz, PROT_WRITE, MAP_FILE | MAP_SHARED, fildes, 0);
	if (addr != MAP_FAILED) {
		printf("mmap() didn't fail (%p != %p)\n", addr, MAP_FAILED);
		if (munmap(addr, page_sz) != 0) {
			printf("munmap() failed\n");
		}
		exit(EXIT_FAILURE);
	}
	if (errno == EBADF) {
		printf("test succeeded\n");
	} else {
		printf("mmap failed with an invalid errno, error number %d\n", errno);
	}
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
		printf("opening %s failed\n", TEMPFILE);
		exit(EXIT_FAILURE);
	}
	if (write(fildes, tst_buff, page_sz) != (int)page_sz) {
		free(tst_buff);
		printf("writing to %s failed\n", TEMPFILE);
		exit(EXIT_FAILURE);
	}
	free(tst_buff);
	if (close(fildes) < 0) {
		printf("closing %s failed\n", TEMPFILE);
		exit(EXIT_FAILURE);
	}
}

