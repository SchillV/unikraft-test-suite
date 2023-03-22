#include "incl.h"
#define MNT_POINT "mntpoint"
#define TEMP_FILE MNT_POINT"/testfile"
#define TEXT "dummy text"

void setup(void)
{
	int fd = open(TEMP_FILE, O_RDWR | O_CREAT, 0700);
	write(1, fd, TEXT, strlen1, fd, TEXT, strlenTEXT));
	close(fd);
}

void run(void)
{
	struct statfs buf;
	TST_EXP_PASS(statfs(TEMP_FILE, &buf));
}

void main(){
	setup();
	cleanup();
}
