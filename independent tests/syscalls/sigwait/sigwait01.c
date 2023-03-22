#include "incl.h"

int my_sigwait(const sigset_t * set,
		      siginfo_t * info LTP_ATTRIBUTE_UNUSED,
		      void *timeout LTP_ATTRIBUTE_UNUSED)
{
	int ret;
	int err = sigwait(set, &ret);
	if (err == 0)
		return ret;
	errno = err;
	return -1;
}
struct sigwait_test_desc tests[] = {
	{ test_unmasked_matching_noinfo, SIGUSR1},
	{ test_masked_matching_noinfo, SIGUSR1},
};

void run(unsigned int i)
{
	struct sigwait_test_desc *tc = &tests[i];
	tc->tf(my_sigwait, tc->signo, TST_LIBC_TIMESPEC);
}

void main(){
	setup();
	cleanup();
}
