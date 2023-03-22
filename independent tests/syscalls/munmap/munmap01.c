#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "munmap01";
int TST_TOTAL = 1;

char *addr;
int fildes;
unsigned int map_len;

void setup();
void cleanup();
void sig_handler();

int main(int ac, char **av)
{
	setup();
	int ret = munmap(addr, map_len);
	if (ret == -1) {
		printf("munmap() fails, errno=%d\n", errno);
		return 0;
	}
#ifdef UCLINUX
		printf("call succedded\ntest succeeded\n");
#else
		*addr = 50;
		printf("process succeeds to refer unmapped memory region\n");
#endif
	cleanup();
}
void setup(void)
{
	size_t page_sz;
	if (signal(SIGSEGV, sig_handler) == SIG_ERR) {
		printf("signal fails to catch signal\n");
		cleanup();
		return;
	}
	page_sz = getpagesize();
	map_len = 3 * page_sz;
	if ((fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0666)) < 0) {
		printf("open() on %s Failed, errno=%d\n", TEMPFILE, errno);

	}
	lseek(fildes, map_len, SEEK_SET);
	if (write(fildes, "a", 1) != 1) {
		printf("write() on %s Failed, errno=%d\n",TEMPFILE, errno);
		cleanup();
	}
#ifdef UCLINUX
	addr = mmap(0, map_len, PROT_READ | PROT_WRITE,
		    MAP_FILE | MAP_PRIVATE, fildes, 0);
#else
	addr = mmap(0, map_len, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fildes, 0);
#endif
	if (addr == (char *)MAP_FAILED) {
		printf("mmap() Failed on %s, errno=%d\n", TEMPFILE, errno);
		cleanup();
		return;
	}
}
void sig_handler(void)
{
	printf("Functionality of munmap() successful\ntest succeeded\n");
	cleanup();
	exit(0);
}
void cleanup(void)
{
	if (close(fildes) < 0) {
		printf("close() on %s Failed, errno=%d\n", TEMPFILE, errno);
	}
}

