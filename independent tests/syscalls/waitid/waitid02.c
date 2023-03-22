#include "incl.h"

siginfo_t *infop;

void run(void)
{
	TST_EXP_FAIL(waitid(P_ALL, 0, infop, WNOHANG), EINVAL);
}

void main(){
	setup();
	cleanup();
}
