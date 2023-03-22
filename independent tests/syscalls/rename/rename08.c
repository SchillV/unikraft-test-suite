#include "incl.h"
#define MNT_POINT "mntpoint"
#define TEMP_FILE "tmpfile"
#define INVALID_PATH ((void *)-1)

void setup(void)
{
	chdir(MNT_POINT);
	touch(TEMP_FILE, 0700, NULL);
}

void run(void)
{
	TST_EXP_FAIL(rename(INVALID_PATH, TEMP_FILE),
				EFAULT);
	TST_EXP_FAIL(rename(TEMP_FILE, INVALID_PATH),
				EFAULT);
}

void main(){
	setup();
	cleanup();
}
