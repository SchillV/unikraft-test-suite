#include "incl.h"

int  verify_times(
{
	struct tms mytimes;
times(&mytimes);
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "times failed");
	else
		tst_res(TPASS, "times(&mytimes) returned %ld", TST_RET);
}

void main(){
	setup();
	cleanup();
}
