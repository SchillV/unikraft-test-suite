#include "incl.h"
#define MAP_SIZE (8 * 1024)

char *addr;

void run(void)
{
	int i;
	memset(addr, 1, MAP_SIZE);
madvise(addr, MAP_SIZE, MADV_DONTNEED);
	if (TST_RET == -1) {
		tst_brk(TBROK | TTERRNO, "madvise(%p, %d, 0x%x) failed",
			addr, MAP_SIZE, MADV_DONTNEED);
	}
	for (i = 0; i < MAP_SIZE; i++) {
		if (addr[i]) {
			tst_res(TFAIL,
				"There are no zero-fill-on-demand pages "
				"for anonymous private mappings");
			return;
		}
	}
	if (i == MAP_SIZE) {
		tst_res(TPASS,
			"There are zero-fill-on-demand pages "
			"for anonymous private mappings");
	}
}

void setup(void)
{
	addr = mmap(NULL, MAP_SIZE,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0);
}

void cleanup(void)
{
	if (addr)
		munmap(addr, MAP_SIZE);
}

void main(){
	setup();
	cleanup();
}
