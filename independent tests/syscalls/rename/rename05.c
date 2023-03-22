#include "incl.h"
#define MNT_POINT "mntpoint"
#define TEMP_FILE "tmpfile"
#define TEMP_DIR "tmpdir"

void setup(void)
{
	chdir(MNT_POINT);
	touch(TEMP_FILE, 0700, NULL);
	mkdir(TEMP_DIR, 00770);
}

void run(void)
{
	TST_EXP_FAIL(rename(TEMP_FILE, TEMP_DIR),
				EISDIR);
}

void main(){
	setup();
	cleanup();
}
