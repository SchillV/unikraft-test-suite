#include "incl.h"
#define INVALID_ID ((kernel_timer_t)-1)

void run(void)
{
tst_syscall(__NR_timer_delete, INVALID_ID);
	if (TST_RET == -1 && TST_ERR == EINVAL) {
		tst_res(TPASS | TTERRNO,
			 "Failed as expected");
	} else {
		tst_res(TFAIL | TTERRNO,
			 "Didn't fail with EINVAL as expected - got");
	}
}

void main(){
	setup();
	cleanup();
}
