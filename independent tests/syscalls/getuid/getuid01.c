#include "incl.h"

int  verify_getuid(
{
	TST_EXP_POSITIVE(GETUID(), "getuid()");
}

void main(){
	setup();
	cleanup();
}
