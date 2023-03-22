#include "incl.h"

void *bad_addr;

int  verify_uname(
{
	TST_EXP_FAIL(uname(bad_addr), EFAULT);
}

void setup(void)
{
	bad_addr = tst_get_bad_addr(NULL);
}

void main(){
	setup();
	cleanup();
}
