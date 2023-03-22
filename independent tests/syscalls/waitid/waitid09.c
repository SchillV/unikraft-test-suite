#include "incl.h"

siginfo_t *infop;

void run(void)
{
	if (!fork())
		exit(0);
	TST_EXP_FAIL(waitid(P_PID, 1, infop, WEXITED), ECHILD);
}

void main(){
	setup();
	cleanup();
}
