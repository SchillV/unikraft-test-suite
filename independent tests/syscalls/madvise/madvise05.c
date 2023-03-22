#include "incl.h"
#define ALLOC_SIZE (32 * 1024 * 1024)

int  verify_madvise(
{
	void *p;
	p = mmap(NULL, ALLOC_SIZE, PROT_READ,
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
mprotect(p, ALLOC_SIZE, PROT_NONE);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "mprotect failed");
madvise(p, ALLOC_SIZE, MADV_WILLNEED);
	munmap(p, ALLOC_SIZE);
	if (TST_RET == 0) {
		tst_res(TPASS, "issue has not been reproduced");
		return;
	}
	if (TST_ERR == EBADF)
		tst_brk(TCONF, "CONFIG_SWAP=n");
	else
		tst_brk(TBROK | TTERRNO, "madvise failed");
}

void main(){
	setup();
	cleanup();
}
