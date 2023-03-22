#include "incl.h"

void run(void)
{
	aio_context_t ctx;
	memset(&ctx, 0xff, sizeof(ctx));
	TST_EXP_FAIL(tst_syscall(__NR_io_destroy, ctx), EINVAL,
		     "io_destroy() with an invalid ctx");
}

void main(){
	setup();
	cleanup();
}
