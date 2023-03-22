#include "incl.h"
* Copyright (c) Ulrich Drepper <drepper@redhat.com>
* Copyright (c) International Business Machines Corp., 2009
*/
* Test Name:	socket02
*
* Description:
* This program tests the new flag SOCK_CLOEXEC and SOCK_NONBLOCK introduced
* in socket() in kernel 2.6.27.
*/

int fd;

struct tcase {
	int type;
	int flag;
	int fl_flag;
	char *des;
} tcases[] = {
	{SOCK_STREAM, 0, F_GETFD, "no close-on-exec"},
	{SOCK_STREAM | SOCK_CLOEXEC, FD_CLOEXEC, F_GETFD, "close-on-exec"},
	{SOCK_STREAM, 0, F_GETFL, "no non-blocking"},
	{SOCK_STREAM | SOCK_NONBLOCK, O_NONBLOCK, F_GETFL, "non-blocking"}
};

int  verify_socket(unsigned int n)
{
	int res;
	struct tcase *tc = &tcases[n];
	fd = socket(PF_INET, tc->type, 0);
	if (fd == -1)
		tst_brk(TFAIL | TERRNO, "socket() failed");
	res = fcntl(fd, tc->fl_flag);
	if (tc->flag != 0 && (res & tc->flag) == 0) {
		tst_res(TFAIL, "socket() failed to set %s flag", tc->des);
		return;
	}
	if (tc->flag == 0 && (res & tc->flag) != 0) {
		tst_res(TFAIL, "socket() failed to set %s flag", tc->des);
		return;
	}
	tst_res(TPASS, "socket() passed to set %s flag", tc->des);
	close(fd);
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
