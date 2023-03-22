#include "incl.h"

char fname[20] = "testfile";

int fd = -1, max_files;

int  verify_fcntl(
{
	pid_t pid;
	int i;
	pid = fork();
	if (pid == 0) {
		for (i = 0; i < max_files; i++) {
			fd = open(fname, O_CREAT | O_RDONLY, 0444);
			if (fd == -1)
				break;
		}
		TST_EXP_FAIL2(fcntl(1, F_DUPFD, 1), EMFILE,
			"fcntl(1, F_DUPFD, 1)");
	}
	tst_reap_children();
}

void setup(void)
{
	max_files = getdtablesize();
}

void cleanup(void)
{
	if (fd > -1)
		close(fd);
	unlink(fname);
}

void main(){
	setup();
	cleanup();
}
