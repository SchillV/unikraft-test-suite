#include "incl.h"
#define MNT_POINT "mntpoint"
#define TEMP_DIR "tmpdir"
#define TEMP_FILE "tmpfile"

void setup(void)
{
	chdir(MNT_POINT);
	mkdir(TEMP_DIR, 00770);
	touch(TEMP_FILE, 0700, NULL);
}

void run(void)
{
	TST_EXP_FAIL(rename(TEMP_DIR, TEMP_FILE),
				ENOTDIR);
}

void main(){
	setup();
	cleanup();
}
