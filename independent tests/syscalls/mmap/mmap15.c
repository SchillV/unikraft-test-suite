#include "incl.h"
char *TCID = "mmap15";
int TST_TOTAL = 1;
#ifdef __ia64__
# define HIGH_ADDR (void *)(0xa000000000000000UL)
#else
# define HIGH_ADDR (void *)(-page_size)
#endif

long page_size;

void setup(void);

void cleanup(void);
int main(int ac, char **av)
{
	int lc, fd;
	void *addr;
#ifdef TST_ABI32
	printf("This test is only for 64bit\n");
	exit(0);
#endif
	setup();
	fd = open("testfile", O_RDWR | O_CREAT, 0666);
	addr = mmap(HIGH_ADDR, page_size, PROT_READ, MAP_SHARED | MAP_FIXED, fd, 0);
	if (addr != MAP_FAILED) {
		printf("mmap into high region succeeded unexpectedly\n");
		munmap(addr, page_size);
		close(fd);
		return 0;
	}
	if (errno != ENOMEM && errno != EINVAL) {
		printf("mmap into high region failed unexpectedly, error number %d\n", errno);
	} else {
		printf("mmap into high region failed as expected, error number %d\ntest succeeded\n", errno);
	}
	close(fd);
}

void setup(void)
{
	page_size = getpagesize();
}

void cleanup(void)
{
}

