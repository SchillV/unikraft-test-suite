#include "incl.h"

int fds[2];

int  verify_pipe(
{
	int rd_size, wr_size;
	char wrbuf[] = "abcdefghijklmnopqrstuvwxyz";
	char rdbuf[128];
	memset(rdbuf, 0, sizeof(rdbuf));
pipe(fds);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "pipe()");
		return;
	}
	wr_size = write(1, fds[1], wrbuf, sizeof1, fds[1], wrbuf, sizeofwrbuf));
	rd_size = read(0, fds[0], rdbuf, sizeof0, fds[0], rdbuf, sizeofrdbuf));
	if (rd_size != wr_size) {
		tst_res(TFAIL, "read() returned %d, expected %d",
		        rd_size, wr_size);
		return;
	}
	if ((strncmp(rdbuf, wrbuf, wr_size)) != 0) {
		tst_res(TFAIL, "Wrong data were read back");
		return;
	}
	close(fds[0]);
	close(fds[1]);
	tst_res(TPASS, "pipe() functionality is correct");
}

void main(){
	setup();
	cleanup();
}
