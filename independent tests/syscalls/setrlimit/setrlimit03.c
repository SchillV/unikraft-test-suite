#include "incl.h"
#if !defined(NR_OPEN)
# define NR_OPEN (1024*1024)
#endif
#define NR_OPEN_PATH "/proc/sys/fs/nr_open"

struct rlimit rlim1, rlim2;

unsigned int nr_open = NR_OPEN;

struct tcase {
	struct rlimit *rlimt;
	int exp_err;
} tcases[] = {
	{&rlim1, EPERM},
	{&rlim2, EINVAL}
};

int  verify_setrlimit(unsigned int n)
{
	struct tcase *tc = &tcases[n];
setrlimit(RLIMIT_NOFILE, tc->rlimt);
	if (TST_RET != -1) {
		tst_res(TFAIL, "call succeeded unexpectedly "
			"(nr_open=%u rlim_cur=%lu rlim_max=%lu)", nr_open,
			(unsigned long)(tc->rlimt->rlim_cur),
			(unsigned long)(tc->rlimt->rlim_max));
		return;
	}
	if (TST_ERR != tc->exp_err) {
		tst_res(TFAIL | TTERRNO, "setrlimit() should fail with %s, got",
			tst_strerrno(tc->exp_err));
	} else {
		tst_res(TPASS | TTERRNO, "setrlimit() failed as expected");
	}
}

void setup(void)
{
	if (!access(NR_OPEN_PATH, F_OK))
		file_scanf(NR_OPEN_PATH, "%u", &nr_open);
	getrlimit(RLIMIT_NOFILE, &rlim1);
	rlim2.rlim_max = rlim1.rlim_cur;
	rlim2.rlim_cur = rlim1.rlim_max + 1;
	rlim1.rlim_max = nr_open + 1;
}

void main(){
	setup();
	cleanup();
}
