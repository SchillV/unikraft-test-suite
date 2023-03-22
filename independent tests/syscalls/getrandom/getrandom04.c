#include "incl.h"

int  verify_getrandom(
{
	char buf[128];
	struct rlimit lold, lnew;
	getrlimit(RLIMIT_NOFILE, &lold);
	lnew.rlim_max = lold.rlim_max;
	lnew.rlim_cur = 3;
	setrlimit(RLIMIT_NOFILE, &lnew);
tst_syscall(__NR_getrandom, buf, 100, 0);
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "getrandom failed");
	else
		tst_res(TPASS, "getrandom returned %ld", TST_RET);
	setrlimit(RLIMIT_NOFILE, &lold);
}

void main(){
	setup();
	cleanup();
}
