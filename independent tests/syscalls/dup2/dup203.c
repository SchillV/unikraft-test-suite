#include "incl.h"

char filename0[40], filename1[40];

int fd0 = -1, fd1 = -1;

struct tcase {
	char *desc;
	int is_close;
} tcases[] = {
	{"Test duping over an open fd", 0},
	{"Test duping over a close fd", 1},
};

void run(unsigned int i)
{
	int fd2, rval;
	char buf[40];
	struct tcase *tc = tcases + i;
	tst_res(TINFO, "%s", tc->desc);
	fd0 = creat(filename0, 0666);
	write(1, fd0, filename0, strlen1, fd0, filename0, strlenfilename0));
	close(fd0);
	fd1 = creat(filename1, 0666);
	write(1, fd1, filename1, strlen1, fd1, filename1, strlenfilename1));
	fd0 = open(filename0, O_RDONLY);
	fcntl(fd0, F_SETFD, 1);
	if (tc->is_close) {
		rval = fd1;
		close(rval);
	}
dup2(fd0, fd1);
	fd2 = TST_RET;
	if (TST_RET == -1) {
		tst_res(TFAIL, "call failed unexpectedly");
		goto free;
	}
	if (fd1 != fd2) {
		tst_res(TFAIL, "file descriptors don't match");
		goto free;
	}
	memset(buf, 0, sizeof(buf));
	read(0, fd2, buf, sizeof0, fd2, buf, sizeofbuf));
	if (strcmp(buf, filename0) != 0)
		tst_res(TFAIL, "read from file got bad data");
	else
		tst_res(TPASS, "test the content of file is correct");
	rval = fcntl(fd2, F_GETFD);
	if (rval != 0)
		tst_res(TFAIL, "the FD_CLOEXEC flag is %#x, expected 0x0",
			rval);
	else
		tst_res(TPASS, "test the FD_CLOEXEC flag is correct");
free:
	close(fd0);
	close(fd1);
	unlink(filename0);
	unlink(filename1);
}

void setup(void)
{
	int pid;
	pid = getpid();
	sprintf(filename0, "dup203.file0.%d\n", pid);
	sprintf(filename1, "dup203.file1.%d\n", pid);
}

void cleanup(void)
{
	close(fd0);
	close(fd1);
}

void main(){
	setup();
	cleanup();
}
