#include "incl.h"
#define MAX_SIZE 256

unsigned int sizes[] = {
	1,
	2,
	3,
	7,
	8,
	15,
	22,
	64,
	127,
};

int  verify_getrandom(unsigned int n)
{
	char buf[MAX_SIZE];
tst_syscall(__NR_getrandom, buf, sizes[n], 0);
	if (TST_RET != sizes[n]) {
		tst_res(TFAIL | TTERRNO, "getrandom returned %li, expected %u",
			TST_RET, sizes[n]);
	} else {
		tst_res(TPASS, "getrandom returned %ld", TST_RET);
	}
}

void main(){
	setup();
	cleanup();
}
