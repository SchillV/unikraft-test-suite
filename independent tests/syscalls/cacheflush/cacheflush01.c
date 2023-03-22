#include "incl.h"
#if __NR_cacheflush != __LTP__NR_INVALID_SYSCALL
#ifndef   ICACHE
# define   ICACHE   (1<<0)
#endif
#ifndef   DCACHE
# define   DCACHE   (1<<1)
#endif
#ifndef   BCACHE
# define   BCACHE   (ICACHE|DCACHE)
#endif
#define CACHE_DESC(x) .cache = x, .desc = #x

struct test_case_t {
	int cache;
	const char *desc;
} test_cases[] = {
	{ CACHE_DESC(ICACHE) },
	{ CACHE_DESC(DCACHE) },
	{ CACHE_DESC(BCACHE) },
};

char *addr;

void setup(void)
{
	addr = malloc(getpagesizegetpagesize));
}

void test_cacheflush(unsigned int i)
{
	struct test_case_t *tc = &test_cases[i];
	TST_EXP_PASS(tst_syscall(__NR_cacheflush, addr, getpagesize(), tc->cache),
	             "%s", tc->desc);
}

void main(){
	setup();
	cleanup();
}
