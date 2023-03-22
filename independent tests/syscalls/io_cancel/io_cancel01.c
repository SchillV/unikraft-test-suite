#include "incl.h"

void run(void)
{
	aio_context_t ctx;
	memset(&ctx, 0, sizeof(ctx));
	TST_EXP_FAIL(tst_syscall(__NR_io_cancel, ctx, NULL, NULL), EFAULT);
}

void main(){
	setup();
	cleanup();
}
