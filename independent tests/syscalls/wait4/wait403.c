#include "incl.h"
#define _USE_BSD

void run(void)
{
	int status = 1;
	struct rusage rusage;
	TST_EXP_FAIL2(wait4(INT_MIN, &status, 0, &rusage), ESRCH,
		      "wait4 fails with ESRCH");
}

void main(){
	setup();
	cleanup();
}
