#include "incl.h"
#define MNT_POINT "mntpoint"
#define DIR1 "dir1"
#define DIR2 DIR1"/dir2"

void setup(void)
{
	chdir(MNT_POINT);
	mkdir(DIR1, 00770);
	mkdir(DIR2, 00770);
}

void run(void)
{
	TST_EXP_FAIL(rename(DIR1, DIR2),
				EINVAL);
}

void main(){
	setup();
	cleanup();
}
