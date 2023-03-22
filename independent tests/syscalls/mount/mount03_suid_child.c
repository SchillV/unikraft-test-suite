#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(void)
{
	tst_reinit();
	TST_EXP_FAIL(setreuid(getuid(), 0), EPERM);
	return 0;
}

