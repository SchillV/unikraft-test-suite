#include "incl.h"

void run(void)
{
	int pagesize_sysconf;
	pagesize_sysconf = sysconf(_SC_PAGESIZE);
	TST_EXP_VAL(getpagesize(), pagesize_sysconf);
}

void main(){
	setup();
	cleanup();
}
