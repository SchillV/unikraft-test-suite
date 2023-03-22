#include "incl.h"

void *brk_variants(void *addr)
{
	void *brk_addr;
	brk(addr);
	brk_addr = (void *)sbrk(0);
	return brk_addr;
}

int brk_down_vmas(void)
{
	void *brk_addr;
	printf("[I] Testing libc variant\n");
	brk_addr = (void *)sbrk(0);
	if (brk_addr == (void *)-1) {
		printf("sbrk() not implemented\n");
		return 0;
	} if (brk(brk_addr) != 0) {
		printf("brk() not implemented\n");
		return 0;
	}
	unsigned long page_size = getpagesize();
	void *addr = brk_addr + page_size;
	if (brk_variants(addr) < addr) {
		printf("Cannot expand brk() by page size, error number %d\n", errno);
		return 0;
	}
	addr += page_size;
	if (brk_variants(addr) < addr) {
		printf("Cannot expand brk() by 2x page size, error number %d\n", errno);
		return 0;
	}
	if (mprotect(addr - page_size, page_size, PROT_READ)) {
		printf("Cannot mprotect new VMA, error number %d\n", errno);
		return 0;
	}
	addr += page_size;
	if (brk_variants(addr) < addr) {
		printf("Cannot expand brk() after mprotect, error number %d\n", errno);
		return 0;
	}
	if (brk_variants(brk_addr) != brk_addr) {
		printf("Cannot restore brk() to start address, error number %d\n", errno);
		return 0;
	}
	printf("munmap at least two VMAs of brk() passed\n");
	return 1;
}

void main(){
	if(brk_down_vmas())
		printf("test succeeded\n");
}
