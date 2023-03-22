#include "incl.h"
#define mapsize (1 << 14)
char *TCID = "mmap09";
int TST_TOTAL = 3;

int fd;

char *maddr;

struct test_case_t {
	off_t newsize;
	char *desc;
} TC[] = {
	{mapsize - 8192, "ftruncate mmaped file to a smaller size"},
	{mapsize + 1024, "ftruncate mmaped file to a larger size"},
	{0, "ftruncate mmaped file to 0 size"},
};

void setup(void);

void cleanup(void);
int main(int argc, char **argv)
{
	int i, ok = 1;
	setup();
	for (i = 0; i < TST_TOTAL; i++) {
		int ret = ftruncate(fd, TC[i].newsize);
		if (ret == -1) {
			printf("%s, error number %d\n", TC[i].desc, errno);
			ok = 0;
		} else {
			printf("%s\n", TC[i].desc);
		}
	}
	cleanup();
	if(ok)
		printf("test succeeded\n");
}

void setup(void)
{
	if ((fd = open("mmaptest", O_RDWR | O_CREAT, 0666)) < 0){
		printf("opening mmaptest failed, error number %d", errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	if (ftruncate(fd, mapsize) < 0){
		printf("ftruncate file failed, error number %d", errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	maddr = mmap(0, mapsize, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0);
	if (maddr == MAP_FAILED){
		printf("mmapping mmaptest failed, error number %d\n", errno);
		cleanup();
		exit(EXIT_FAILURE);
	}
	memset(maddr, 'A', mapsize);
}

void cleanup(void)
{
	munmap(maddr, mapsize);
	close(fd);
}

