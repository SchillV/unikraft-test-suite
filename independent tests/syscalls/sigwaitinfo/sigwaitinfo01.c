#include "incl.h"

int my_sigwaitinfo(const sigset_t * set, siginfo_t * info,
			  void *timeout LTP_ATTRIBUTE_UNUSED)
{
	return sigwaitinfo(set, info);
}
struct sigwait_test_desc tests[] = {
	{ test_empty_set, SIGUSR1},
	{ test_unmasked_matching, SIGUSR1},
	{ test_masked_matching, SIGUSR1},
	{ test_unmasked_matching_noinfo, SIGUSR1},
	{ test_masked_matching_noinfo, SIGUSR1},
	{ test_bad_address, SIGUSR1},
	{ test_bad_address2, SIGUSR1},
};

void run(unsigned int i)
{
	struct sigwait_test_desc *tc = &tests[i];
	tc->tf(my_sigwaitinfo, tc->signo, TST_LIBC_TIMESPEC);
}

void main(){
	setup();
	cleanup();
}
