#include "incl.h"
#define FILENAME "fchown01_testfile"
#define MODE 0700

int fd;

uid_t uid;

gid_t gid;

void run(void)
{
	TST_EXP_PASS(FCHOWN(fd, uid, gid),
		"fchown(%i, %i, %i)", fd, uid, gid);
}

void setup(void)
{
	UID16_CHECK(uid = geteuid(), "fchown");
	GID16_CHECK(gid = getegid(), "fchown");
	fd = open(FILENAME, O_RDWR | O_CREAT, MODE);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
