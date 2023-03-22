#include "incl.h"
#define MNT_POINT "mntpoint"
#define TEMP_FILE "tmpfile"

char long_path[NAME_MAX + 1] = {[0 ... NAME_MAX] = 'a'};

void setup(void)
{
	chdir(MNT_POINT);
	touch(TEMP_FILE, 0700, NULL);
}

void run(void)
{
	TST_EXP_FAIL(rename(TEMP_FILE, long_path),
				ENAMETOOLONG);
}

void main(){
	setup();
	cleanup();
}
