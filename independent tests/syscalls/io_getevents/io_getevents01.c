#include "incl.h"

void run(void)
{
	aio_context_t ctx;
	memset(&ctx, 0, sizeof(ctx));
	TST_EXP_FAIL2(tst_syscall(__NR_io_getevents, ctx, 0, 0, NULL, NULL), EINVAL,
		"io_getevents syscall with invalid ctx");
}

void main(){
	setup();
	cleanup();
}
