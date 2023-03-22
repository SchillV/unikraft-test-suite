#include "incl.h"
#define BUF_SIZE	512

int fildes[2];

int  verify_pollout(
{
	struct pollfd outfds[] = {
		{.fd = fildes[1], .events = POLLOUT},
	};
poll(outfds, 1, -1);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "poll() POLLOUT failed");
		return;
	}
	if (outfds[0].revents != POLLOUT) {
		tst_res(TFAIL | TTERRNO, "poll() failed to set POLLOUT");
		return;
	}
	tst_res(TPASS, "poll() POLLOUT");
}

int  verify_pollin(
{
	char write_buf[] = "Testing";
	char read_buf[BUF_SIZE];
	struct pollfd infds[] = {
		{.fd = fildes[0], .events = POLLIN},
	};
	write(1, fildes[1], write_buf, sizeof1, fildes[1], write_buf, sizeofwrite_buf));
poll(infds, 1, -1);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "poll() POLLIN failed");
		goto end;
	}
	if (infds[0].revents != POLLIN) {
		tst_res(TFAIL, "poll() failed to set POLLIN");
		goto end;
	}
	tst_res(TPASS, "poll() POLLIN");
end:
	read(1, fildes[0], read_buf, sizeof1, fildes[0], read_buf, sizeofwrite_buf));
}
int  verify_poll(unsigned int n)
{
	switch (n) {
	case 0:
int 		verify_pollout();
	break;
	case 1:
int 		verify_pollin();
	break;
	}
}

void setup(void)
{
	pipe(fildes);
}

void cleanup(void)
{
	if (fildes[0] > 0)
		close(fildes[0]);
	if (fildes[1] > 0)
		close(fildes[1]);
}

void main(){
	setup();
	cleanup();
}
