#include "incl.h"
* Copyright (c) Ulrich Drepper <drepper@redhat.com>
* Copyright (c) International Business Machines Corp., 2009
*/
* Test Name:	socketpair02
*
* Description:
* This Program tests the new flag SOCK_CLOEXEC and SOCK_NONBLOCK introduced
* in socketpair() in kernel 2.6.27.
*/

int fds[2];

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

int  verify_socketpair(unsigned int n)
{
	int res, i;
	struct tcase *tc = &tcases[n];
socketpair(PF_UNIX, tc->type, 0, fds);
	if (TST_RET == -1)
		tst_brk(TFAIL | TTERRNO, "socketpair() failed");
	for (i = 0; i < 2; i++) {
		res = fcntl(fds[i], tc->fl_flag);
		if (tc->flag != 0 && (res & tc->flag) == 0) {
			tst_res(TFAIL, "socketpair() failed to set %s flag for fds[%d]",
				tc->des, i);
			goto ret;
		}
		if (tc->flag == 0 && (res & tc->flag) != 0) {
			tst_res(TFAIL, "socketpair() failed to set %s flag for fds[%d]",
				tc->des, i);
			goto ret;
		}
	}
	tst_res(TPASS, "socketpair() passed to set %s flag", tc->des);
ret:
	close(fds[0]);
	close(fds[1]);
}

void cleanup(void)
{
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[1] > 0)
		close(fds[1]);
}

void main(){
	setup();
	cleanup();
}
