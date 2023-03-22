#include "incl.h"

int *opened_fds, num_opened_fds;

int  verify_creat(
{
creat("filename", 0666);
	if (TST_RET != -1) {
		tst_res(TFAIL, "call succeeded unexpectedly");
		close(TST_RET);
		return;
	}
	if (TST_ERR == EMFILE)
		tst_res(TPASS, "creat() failed with EMFILE");
	else
		tst_res(TFAIL | TTERRNO, "Expected EMFILE");
}

void setup(void)
{
	int max_open;
	int fd;
	char fname[PATH_MAX];
	max_open = getdtablesize();
	tst_res(TINFO, "getdtablesize() = %d", max_open);
	opened_fds = malloc(max_open * sizeofmax_open * sizeofint));
	do {
		snprintf(fname, sizeof(fname), "creat05_%d", num_opened_fds);
		fd = creat(fname, 0666);
		opened_fds[num_opened_fds++] = fd;
	} while (fd < max_open - 1);
	tst_res(TINFO, "Opened additional #%d fds", num_opened_fds);
}

void cleanup(void)
{
	int i;
	for (i = 0; i < num_opened_fds; i++) {
		if (close(opened_fds[i])) {
			tst_res(TWARN | TERRNO, "close(%i) failed",
				opened_fds[i]);
		}
	}
	free(opened_fds);
}

void main(){
	setup();
	cleanup();
}
