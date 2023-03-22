#include "incl.h"

gid_t gid;

void run(void)
{
	TST_EXP_PASS(SETGID(gid));
}

void setup(void)
{
	gid = getgid();
	GID16_CHECK(gid, setgid);
}

void main(){
	setup();
	cleanup();
}
