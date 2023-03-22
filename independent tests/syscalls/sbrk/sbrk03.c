#include "incl.h"

void sbrk_test(void)
{
#if defined(__s390__) && defined(TST_ABI32)
	void *ret1, *ret2;
	tst_res(TINFO, "initial brk: %d", brk((void *)0x10000000));
	tst_res(TINFO, "sbrk increm: %p", sbrk(0x10000000));
	ret1 = sbrk(0);
	tst_res(TINFO, "sbrk increm: %p", sbrk(0x70000000));
	ret2 = sbrk(0);
	if (ret1 != ret2) {
		tst_res(TFAIL, "Bug! sbrk: %p", ret2);
		return;
	}
int 	tst_res(TPASS, "sbrk verify: %p", ret2);
#else
	tst_res(TCONF, "Only works in 32bit on s390 series system");
#endif
}

void main(){
	setup();
	cleanup();
}
