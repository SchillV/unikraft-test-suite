#include "incl.h"

int  verify_fchdir(
{
	const int bad_fd = -5;
	TST_EXP_FAIL(fchdir(bad_fd), EBADF);
}

void main(){
	setup();
	cleanup();
}
