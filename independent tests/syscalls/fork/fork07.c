#include "incl.h"
#define NFORKS 100
#define TESTFILE "testfile_fork07"

int fd;

char buf;

void run(void)
{
	int ret, i;
	fd = open(TESTFILE, O_RDONLY);
	for (i = 0; i < NFORKS; ++i) {
		if (!fork()) {
			read(1, fd, &buf, 1);
			if (buf != 'a')
				tst_res(TFAIL, "%6d: read '%c' instead of 'a'",
					getpid(), buf);
			exit(0);
		}
	}
	tst_reap_children();
	ret = read(fd, &buf, 1);
	if (ret == 0)
		tst_res(TPASS, "read the end of file correctly");
	else
		tst_res(TFAIL, "read() returns %d, expected 0", ret);
	close(fd);
}

void setup(void)
{
	tst_fill_file(TESTFILE, 'a', NFORKS, 1);
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
