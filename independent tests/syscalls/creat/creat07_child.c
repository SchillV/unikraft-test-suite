#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(void)
{
	tst_reinit();
	TST_CHECKPOINT_WAKE(0);
	pause();
	return 0;
}

