#include "incl.h"

void run(void)
{
	TST_EXP_FAIL2(kill(INT_MIN, 0), ESRCH,
		      "kill(INT_MIN, ...) fails with ESRCH");
}

void main(){
	setup();
	cleanup();
}
