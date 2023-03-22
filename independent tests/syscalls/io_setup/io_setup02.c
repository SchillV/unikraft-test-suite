#include "incl.h"

void run(void)
{
	aio_context_t ctx;
	TST_EXP_FAIL(tst_syscall(__NR_io_setup, 1, NULL), EFAULT,
		     "io_setup() when ctxp is NULL");
	memset(&ctx, 1, sizeof(ctx));
	TST_EXP_FAIL(tst_syscall(__NR_io_setup, 1, &ctx), EINVAL,
		     "io_setup() when ctxp is not initialized to 0");
	memset(&ctx, 0, sizeof(ctx));
	TST_EXP_FAIL(tst_syscall(__NR_io_setup, -1, &ctx), EINVAL,
		     "io_setup() when nr_events is -1");
	unsigned aio_max = 0;
	if (!access("/proc/sys/fs/aio-max-nr", F_OK)) {
		file_scanf("/proc/sys/fs/aio-max-nr", "%u", &aio_max);
		TST_EXP_FAIL(tst_syscall(__NR_io_setup, aio_max + 1, &ctx), EAGAIN,
			     "io_setup() when nr_events exceeds the limit");
	} else {
		tst_res(TCONF, "the aio-max-nr file did not exist");
	}
	TST_EXP_PASS(tst_syscall(__NR_io_setup, 1, &ctx),
		     "io_setup() when both nr_events and ctxp are valid");
	TST_EXP_PASS_SILENT(tst_syscall(__NR_io_destroy, ctx));
}

void main(){
	setup();
	cleanup();
}
