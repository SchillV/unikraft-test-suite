#include "incl.h"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
int fd[2];

void run(void)
{
dup(fd[0]);
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO,
			 "dup of read side of pipe failed");
	else {
		tst_res(TPASS,
			 "dup(%d) read side of syspipe returned %ld",
			 fd[0], TST_RET);
		close(TST_RET);
	}
dup(fd[1]);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO,
			 "dup of write side of pipe failed");
	} else {
		tst_res(TPASS,
			 "dup(%d) write side of syspipe returned %ld",
			 fd[1], TST_RET);
		close(TST_RET);
	}
}
void setup(void)
{
	fd[0] = -1;
	pipe(fd);
}

void main(){
	setup();
	cleanup();
}
