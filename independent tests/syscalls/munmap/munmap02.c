#include "incl.h"
#define TEMPFILE	"mmapfile"
char *TCID = "munmap02";
int TST_TOTAL = 1;

size_t page_sz;
char *addr;			/* addr of memory mapped region */
int fildes;			/* file descriptor for tempfile */
unsigned int map_len;		/* length of the region to be mapped */

void setup();			/* Main setup function of test */
void cleanup();			/* cleanup function for the test */
void sig_handler();

#ifndef UCLINUX
int main(int ac, char **av)
{
		setup();
	int ret = munmap(addr, map_len);
	if (ret == -1) {
		printf("munmap() fails, errno=%d\n", errno);
		return 0;
	}
	*addr = 50;
	printf("process succeeds to refer unmapped memory region\n");
	cleanup();
}
#else
int main(void)
{
	printf("[I] munmap02 test is not available on uClinux\n");
}
#endif
void setup(void)
{
	if (signal(SIGSEGV, sig_handler) == SIG_ERR) {
		printf("signal fails to catch signal\n");
		cleanup();
		exit(0);
	}
	page_sz = getpagesize();
	map_len = 3 * page_sz;
	if ((fildes = open(TEMPFILE, O_RDWR | O_CREAT, 0666)) < 0) {
		printf("open() on %s Failed, errno=%d\n", TEMPFILE, errno);
		cleanup();
		exit(0);
	}
	lseek(fildes, map_len, SEEK_SET);
	if (write(fildes, "a", 1) != 1) {
		printf("write() on %s Failed, errno=%d\n", TEMPFILE, errno);
		cleanup();
		exit(0);
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
		exit(0);
	}
	addr = (char *)((long)addr + page_sz);
	map_len = map_len - page_sz;
}
void sig_handler(void)
{
	printf("Functionality of munmap() successful\ntest succeeded\n");
	cleanup();
	exit(0);
}
void cleanup(void)
{
	addr = (char *)((long)addr - page_sz);
	map_len = map_len - page_sz;
	munmap(addr, map_len);
	close(fildes);
}

