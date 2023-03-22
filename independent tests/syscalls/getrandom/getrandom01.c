#include "incl.h"

int modes[] = {0, GRND_RANDOM, GRND_NONBLOCK,
		      GRND_RANDOM | GRND_NONBLOCK};

int  verify_getrandom(unsigned int n)
{
tst_syscall(__NR_getrandom, NULL, 100, modes[n]);
	if (TST_RET == -1) {
		tst_res(TPASS | TTERRNO, "getrandom returned %ld",
			TST_RET);
	} else {
		tst_res(TFAIL | TTERRNO, "getrandom failed");
	}
}

void main(){
	setup();
	cleanup();
}
