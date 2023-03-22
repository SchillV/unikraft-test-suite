#include "incl.h"
#define TESTFILE "testfile_fork08"

int fd;

char buf;

void run(void)
{
	int ret;
	fd = open(TESTFILE, O_RDONLY);
	if (!fork()) {
		close(fd);
		exit(0);
	}
	if (!fork()) {
		read(1, fd, &buf, 1);
		if (buf != 'a')
			tst_res(TFAIL, "%6d: read '%c' instead of 'a'",
				getpid(), buf);
		close(fd);
		exit(0);
	}
	tst_reap_children();
	ret = read(fd, &buf, 1);
	if (ret == 0)
		tst_res(TPASS, "read the end of file correctly");
	else
		tst_res(TFAIL | TERRNO, "read() returns %d, expected 0", ret);
	close(fd);
}

void setup(void)
{
	tst_fill_file(TESTFILE, 'a', 1, 1);
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
