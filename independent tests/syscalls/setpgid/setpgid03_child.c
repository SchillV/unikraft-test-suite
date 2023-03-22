#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(void)
{
	tst_reinit();
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
	return 0;
}

