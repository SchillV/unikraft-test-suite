#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(void)
{
	tst_reinit();
	tst_res(TFAIL, "execveat() passes unexpectedly");
	return 0;
}

