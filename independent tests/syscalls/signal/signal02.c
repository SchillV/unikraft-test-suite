#include "incl.h"

int sigs[] = { _NSIG + 1, SIGKILL, SIGSTOP };

void do_test(unsigned int n)
{
	TST_EXP_FAIL2((long)signal(sigs[n], SIG_IGN), EINVAL);
}

void main(){
	setup();
	cleanup();
}
