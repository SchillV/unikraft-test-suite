#include "incl.h"

int pipe_max_unpriv;

int test_max_unpriv;

int test_max_priv;

struct passwd *pw;

struct tcase {
	int *exp_sz;
	int exp_usr;
	char *des;
} tcases[] = {
	{&test_max_unpriv, 1, "an unprivileged user"},
	{&test_max_priv, 0, "a privileged user"}
};

void setup(void)
{
	test_max_unpriv = getpagesize();
	test_max_priv = test_max_unpriv * 16;
	if (!access("/proc/sys/fs/pipe-max-size", F_OK)) {
		file_scanf("/proc/sys/fs/pipe-max-size", "%d",
				&pipe_max_unpriv);
		fprintf("/proc/sys/fs/pipe-max-size", "%d",
				test_max_unpriv);
	} else {
		tst_brk(TCONF, "/proc/sys/fs/pipe-max-size doesn't exist");
	}
	pw = getpwnam("nobody");
}

void cleanup(void)
{
	fprintf("/proc/sys/fs/pipe-max-size", "%d", pipe_max_unpriv);
}

int int verify_pipe_size(int exp_pip_sz, char *desp)
{
	int get_size;
	int fds[2];
	pipe(fds);
	get_size = fcntl(fds[1], F_GETPIPE_SZ);
	if (get_size == -1) {
		tst_res(TFAIL | TERRNO, "fcntl(2) with F_GETPIPE_SZ failed");
		goto end;
	}
	if (get_size != exp_pip_sz) {
		tst_res(TFAIL, "%s init the capacity of a pipe to %d "
			"unexpectedly, expected %d", desp, get_size,
			exp_pip_sz);
	} else {
		tst_res(TPASS, "%s init the capacity of a pipe to %d "
			"successfully", desp, exp_pip_sz);
	}
end:
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[1] > 0)
		close(fds[1]);
	exit(0);
}

void do_test(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (!fork()) {
		if (tc->exp_usr)
			setuid(pw->pw_uid);
int 		verify_pipe_size(*tc->exp_sz, tc->des);
	}
	tst_reap_children();
}

void main(){
	setup();
	cleanup();
}
