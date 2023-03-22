#include "incl.h"
char *TCID = "munmap03";

size_t page_sz;
char *global_addr;
size_t global_maplen;
int ret;
void setup(void);
void cleanup(void);

void test_einval1(void);
void test_einval2(void);
void test_einval3(void);

void (*testfunc[])(void) = { test_einval1, test_einval2, test_einval3 };
int TST_TOTAL = sizeof(testfunc)/sizeof(testfunc[0]);
int main(int ac, char **av)
{
	int i;
	setup();
	for (i = 0; i < TST_TOTAL; i++)
		(*testfunc[i])();
	cleanup();
}

void setup(void)
{
	page_sz = (size_t)sysconf(_SC_PAGESIZE);
	global_maplen = page_sz * 2;
	global_addr = mmap(NULL, global_maplen, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
}

void check_and_print(int expected_errno)
{
	if (ret == -1) {
		if (errno == expected_errno) {
			printf("failed as expected\ntest succeeded\n");
			exit(0);
		} else {
			printf("failed unexpectedly; expected - %d\n", expected_errno);
		}
	} else {
		printf("munmap succeeded unexpectedly\n");
	}
}

void test_einval1(void)
{
	struct rlimit brkval;
	char *addr;
	size_t map_len;
	getrlimit(RLIMIT_DATA, &brkval);
	addr = (char *)brkval.rlim_max;
	map_len = page_sz * 2;
	ret = munmap(addr, map_len);
	check_and_print(EINVAL);
}

void test_einval2(void)
{
	char *addr = global_addr;
	size_t map_len = 0;
	ret = munmap(addr, map_len);
	check_and_print(EINVAL);
}

void test_einval3(void)
{
	char *addr = (char *)(global_addr + 1);
	size_t map_len = page_sz;
	ret = munmap(addr, map_len);
	check_and_print(EINVAL);
}

void cleanup(void)
{
	if (munmap(global_addr, global_maplen) == -1)
		printf("munmap failed, error number %d", errno);
}

