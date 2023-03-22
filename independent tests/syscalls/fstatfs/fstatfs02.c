#include "incl.h"

struct statfs buf;

int fd;

int bad_fd = -1;

struct test_case_t {
	int *fd;
	struct statfs *sbuf;
	int error;
} tests[] = {
	{&bad_fd, &buf, EBADF},
	{&fd, (void *)-1, EFAULT},
};

int  fstatfs_verify(unsigned int n)
{
	int pid, status;
	pid = fork();
	if (!pid) {
		TST_EXP_FAIL(fstatfs(*tests[n].fd, tests[n].sbuf), tests[n].error, "fstatfs()");
		exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		return;
	if (tests[n].error == EFAULT &&
	    WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV) {
		tst_res(TPASS, "Got SIGSEGV instead of EFAULT");
		return;
	}
	tst_res(TFAIL, "Child %s", tst_strstatus(status));
}

void setup(void)
{
	fd = open("tempfile", O_RDWR | O_CREAT, 0700);
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
