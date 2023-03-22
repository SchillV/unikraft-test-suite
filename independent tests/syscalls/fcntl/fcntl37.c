#include "incl.h"

int fds[2];

unsigned int invalid_value, half_value, sys_value;

struct tcase {
	unsigned int *setvalue;
	int exp_err;
	char *message;
} tcases[] = {
	{&invalid_value, EINVAL, "F_SETPIPE_SZ and size is beyond 1<<31"},
	{&half_value, EBUSY, "F_SETPIPE_SZ and size < data stored in pipe"},
	{&sys_value, EPERM, "F_SETPIPE_SZ and size is over limit for unpriviledged user"},
};

int  verify_fcntl(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	tst_res(TINFO, "%s", tc->message);
fcntl(fds[1], F_SETPIPE_SZ, *(tc->setvalue));
	if (TST_RET != -1) {
		tst_res(TFAIL, "F_SETPIPE_SZ succeed and return %ld", TST_RET);
		return;
	}
	if (TST_ERR == tc->exp_err)
		tst_res(TPASS | TTERRNO, "F_SETPIPE_SZ failed as expected");
	else
		tst_res(TFAIL | TTERRNO, "F_SETPIPE_SZ failed expected %s got",
				tst_strerrno(tc->exp_err));
}

void setup(void)
{
	char *wrbuf;
	unsigned int orig_value;
	pipe(fds);
fcntl(fds[1], F_GETPIPE_SZ);
	if (TST_ERR == EINVAL)
		tst_brk(TCONF, "kernel doesn't support F_GET/SETPIPE_SZ");
	orig_value = TST_RET;
	wrbuf = malloc(orig_value);
	memset(wrbuf, 'x', orig_value);
	write(1, fds[1], wrbuf, orig_value);
	free(wrbuf);
	file_scanf("/proc/sys/fs/pipe-max-size", "%d", &sys_value);
	sys_value++;
	half_value = orig_value / 2;
	invalid_value = (1U << 31) + 1;
}

void cleanup(void)
{
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[1] > 0)
		close(fds[1]);
}

void main(){
	setup();
	cleanup();
}
