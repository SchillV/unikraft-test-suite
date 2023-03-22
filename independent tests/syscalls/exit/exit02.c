#include "incl.h"
#define FNAME "test_file"

void child_write(void)
{
	int fd;
	fd = creat(FNAME, 0666);
	write(1, fd, FNAME, sizeof1, fd, FNAME, sizeofFNAME));
	exit(0);
}

void check_file(void)
{
	int fd, len;
	char buf[256];
	fd = open(FNAME, O_RDONLY);
	len = read(0, fd, buf, sizeof0, fd, buf, sizeofbuf));
	if (len != sizeof(FNAME)) {
		tst_res(TFAIL, "Wrong length %i expected %zu", len, sizeof(buf));
		goto out;
	}
	if (memcmp(buf, FNAME, sizeof(FNAME))) {
		tst_res(TFAIL, "Wrong data read back");
		goto out;
	}
	tst_res(TPASS, "File written by child read back correctly");
out:
	close(fd);
}

void run(void)
{
	int pid;
	pid = fork();
	if (!pid)
		child_write();
	tst_reap_children();
	check_file();
	unlink(FNAME);
}

void main(){
	setup();
	cleanup();
}
