#include "incl.h"

struct timespec tcases[] = {
	{.tv_sec = -5, .tv_nsec = 9999},
	{.tv_sec = 0, .tv_nsec = 1000000000},
	{.tv_sec = 1, .tv_nsec = -100},
};

int  verify_nanosleep(unsigned int n)
{
nanosleep(&tcases[n], NULL);
	if (TST_RET != -1) {
		tst_res(TFAIL,
		        "nanosleep() returned %ld, expected -1", TST_RET);
		return;
	}
	if (TST_ERR != EINVAL) {
		tst_res(TFAIL | TTERRNO,
			"nanosleep() failed,expected EINVAL, got");
		return;
	}
	tst_res(TPASS, "nanosleep() failed with EINVAL");
}

void main(){
	setup();
	cleanup();
}
