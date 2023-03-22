#include "incl.h"
#define FILENAME "chown01_testfile"

int uid, gid;

void run(void)
{
	TST_EXP_PASS(CHOWN(FILENAME, uid, gid), "chown(%s,%d,%d)",
		     FILENAME, uid, gid);
}

void setup(void)
{
	UID16_CHECK((uid = geteuid()), "chown");
	GID16_CHECK((gid = getegid()), "chown");
	fprintf(FILENAME, "davef");
}

void main(){
	setup();
	cleanup();
}
