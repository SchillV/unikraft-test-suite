#include "incl.h"
#define TEMP_DIR "testdir"
#define TEMP_DIR2 TEMP_DIR"/subdir"

void setup(void)
{
	struct passwd *ltpuser;
	mkdir(TEMP_DIR, 0444);
	mkdir(TEMP_DIR2, 0444);
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
}

void run(void)
{
	struct statfs buf;
	TST_EXP_FAIL(statfs(TEMP_DIR2, &buf), EACCES);
}

void main(){
	setup();
	cleanup();
}
