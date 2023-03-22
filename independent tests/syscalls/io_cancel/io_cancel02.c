#include "incl.h"
#ifdef HAVE_LIBAIO

void run(void)
{
	io_context_t ctx;
	memset(&ctx, 0, sizeof(ctx));
io_cancel(ctx, NULL, NULL);
	if (TST_RET == 0) {
		tst_res(TFAIL, "io_cancel() succeeded unexpectedly");
		return;
	}
	if (TST_RET == -EFAULT) {
		tst_res(TPASS, "io_cancel() failed with EFAULT");
		return;
	}
	tst_res(TFAIL, "io_cancel() failed unexpectedly %s (%ld) expected EFAULT",
		tst_strerrno(-TST_RET), -TST_RET);
}

void main(){
	setup();
	cleanup();
}
