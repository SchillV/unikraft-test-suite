#include "incl.h"
int *fd;
int nfds;

void run(void)
{
dup(fd[0]);
	if (TST_RET < -1) {
		tst_res(TFAIL, "Invalid dup() return value %ld", TST_RET);
		return;
	}
	if (TST_RET == -1) {
		if (TST_ERR == EMFILE)
			tst_res(TPASS | TTERRNO, "dup() failed as expected");
		else
			tst_res(TFAIL | TTERRNO, "dup() failed unexpectedly");
		return;
	}
	tst_res(TFAIL, "dup() succeeded unexpectedly");
	close(TST_RET);
}

void setup(void)
{
	long maxfds;
	maxfds = sysconf(_SC_OPEN_MAX);
	if (maxfds == -1)
		tst_brk(TBROK, "sysconf(_SC_OPEN_MAX) failed");
	fd = malloc(maxfds * sizeofmaxfds * sizeofint));
	fd[0] = open("dupfile", O_RDWR | O_CREAT, 0700);
	for (nfds = 1; nfds < maxfds; nfds++) {
		fd[nfds] = dup(fd[0]);
		if (fd[nfds] >= maxfds - 1)
			break;
	}
}

void cleanup(void)
{
	int i;
	for (i = 0; i < nfds; i++)
		close(fd[i]);
}

void main(){
	setup();
	cleanup();
}
