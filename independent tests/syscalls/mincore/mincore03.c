#include "incl.h"
#define NUM_PAGES 3

struct tcase {
	bool mlock;
	int expected_pages;
	char *desc;
} tcases[] = {
	{ false, 0, "untouched pages are not resident"},
	{ true, NUM_PAGES, "locked pages are resident"},
};

int size, page_size;

void *ptr;

void cleanup(void)
{
	if (ptr)
		munmap(ptr, size);
}

void setup(void)
{
	page_size = getpagesize();
	size = page_size * NUM_PAGES;
}

void test_mincore(unsigned int test_nr)
{
	const struct tcase *tc = &tcases[test_nr];
	unsigned char vec[NUM_PAGES];
	int locked_pages;
	int count, mincore_ret;
	ptr = mmap(NULL, size,  PROT_WRITE | PROT_READ, MAP_PRIVATE |  MAP_ANONYMOUS, 0, 0);
	if (tc->mlock)
		mlock(ptr, size);
	mincore_ret = mincore(ptr, size, vec);
	if (mincore_ret == -1)
		tst_brk(TBROK | TERRNO, "mincore failed");
	locked_pages = 0;
	for (count = 0; count < NUM_PAGES; count++)
		if (vec[count] & 1)
			locked_pages++;
	if (locked_pages == tc->expected_pages)
		tst_res(TPASS, "mincore() reports %s", tc->desc);
	else
		tst_res(TFAIL, "mincore reports resident pages as %d, but expected %d",
			locked_pages, tc->expected_pages);
	if (tc->mlock)
		munlock(ptr, size);
	munmap(ptr, size);
	ptr = NULL;
}

void main(){
	setup();
	cleanup();
}
