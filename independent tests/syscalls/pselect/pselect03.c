#include "incl.h"
  * This is basic test for pselect() returning without error.
  */

int fd;

int  verify_pselect(
{
	fd_set readfds;
	struct timespec tv = {0};
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
pselect(fd, &readfds, 0, 0, &tv, NULL);
	if (TST_RET >= 0)
		tst_res(TPASS, "pselect() succeeded retval=%li", TST_RET);
	else
		tst_res(TFAIL | TTERRNO, "pselect() failed unexpectedly");
}

void setup(void)
{
	fd = open("pselect03_file", O_CREAT | O_RDWR, 0777);
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
