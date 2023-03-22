#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(void)
{
	tst_reinit();
	tst_res(TPASS, "execveat_child run as expected");
	return 0;
}

