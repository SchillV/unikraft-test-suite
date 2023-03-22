#include "incl.h"
#ifdef HAVE_LIBAIO

void run(void)
{
	io_context_t ctx;
	memset(&ctx, 0, sizeof(ctx));
io_getevents(ctx, 0, 0, NULL, NULL);
	if (TST_RET == 0) {
		tst_res(TFAIL, "io_getevents() succeeded unexpectedly");
		return;
	}
	if (TST_RET == -EINVAL) {
		tst_res(TPASS, "io_getevents() failed with EINVAL");
		return;
	}
	tst_res(TFAIL, "io_getevents() failed unexpectedly %s (%ld) expected EINVAL",
		tst_strerrno(-TST_RET), -TST_RET);
}

void main(){
	setup();
	cleanup();
}
