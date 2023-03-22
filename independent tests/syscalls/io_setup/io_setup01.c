#include "incl.h"
#ifdef HAVE_LIBAIO

int  verify_failure(unsigned int nr, io_context_t *ctx, int init_val, long exp_err)
{
	if (ctx)
		memset(ctx, init_val, sizeof(*ctx));
io_setup(nr, ctx);
	if (TST_RET == 0) {
		tst_res(TFAIL, "io_setup() passed unexpectedly");
		io_destroy(*ctx);
		return;
	}
	if (TST_RET == -exp_err) {
		tst_res(TPASS, "io_setup() failed as expected, returned -%s",
			tst_strerrno(exp_err));
	} else {
		tst_res(TFAIL, "io_setup() failed unexpectedly, returned -%s "
			"expected -%s", tst_strerrno(-TST_RET),
			tst_strerrno(exp_err));
	}
}

int  verify_success(unsigned int nr, io_context_t *ctx, int init_val)
{
	memset(ctx, init_val, sizeof(*ctx));
io_setup(nr, ctx);
	if (TST_RET == -ENOSYS)
		tst_brk(TCONF | TRERRNO, "io_setup(): AIO not supported by kernel");
	if (TST_RET != 0) {
		tst_res(TFAIL, "io_setup() failed unexpectedly with %li (%s)",
			TST_RET, tst_strerrno(-TST_RET));
		return;
	}
	tst_res(TPASS, "io_setup() passed as expected");
	io_destroy(*ctx);
}

int  verify_io_setup(
{
	io_context_t ctx;
	unsigned int aio_max = 0;
int 	verify_success(1, &ctx, 0);
int 	verify_failure(1, &ctx, 1, EINVAL);
int 	verify_failure(-1, &ctx, 0, EINVAL);
int 	verify_failure(1, NULL, 0, EFAULT);
	if (!access("/proc/sys/fs/aio-max-nr", F_OK)) {
		file_scanf("/proc/sys/fs/aio-max-nr", "%u", &aio_max);
int 		verify_failure(aio_max + 1, &ctx, 0, EAGAIN);
	} else {
		tst_res(TCONF, "the aio-max-nr file did not exist");
	}
}

void main(){
	setup();
	cleanup();
}
