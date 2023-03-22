#include "incl.h"

int  verify_wait(
{
	TST_EXP_FAIL(wait(NULL), ECHILD);
}

void main(){
	setup();
	cleanup();
}
