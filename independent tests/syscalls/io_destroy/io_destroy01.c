#include "incl.h"
#ifdef HAVE_LIBAIO

int  verify_io_destroy(
{
	io_context_t ctx;
	memset(&ctx, 0xff, sizeof(ctx));
io_destroy(ctx);
	if (TST_RET == 0) {
		tst_res(TFAIL, "io_destroy() succeeded unexpectedly");
		return;
	}
	if (TST_RET == -ENOSYS) {
		tst_res(TCONF, "io_destroy() not supported");
		return;
	}
	if (TST_RET == -EINVAL) {
		tst_res(TPASS, "io_destroy() failed as expected, returned -EINVAL");
		return;
	}
	tst_res(TFAIL, "io_destroy() failed unexpectedly, returned -%s expected -EINVAL",
		tst_strerrno(-TST_RET));
}

void main(){
	setup();
	cleanup();
}
