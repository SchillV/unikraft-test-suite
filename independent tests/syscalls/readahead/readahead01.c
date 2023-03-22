#include "incl.h"
#define _GNU_SOURCE
#if defined(__NR_readahead)

void test_bad_fd(void)
{
	char tempname[PATH_MAX] = "readahead01_XXXXXX";
	int fd;
	tst_res(TINFO, "%s -1", __func__);
	TST_EXP_FAIL(readahead(-1, 0, getpagesize()), EBADF);
	tst_res(TINFO, "%s O_WRONLY", __func__);
	fd = mkstemp(tempname);
	if (fd == -1)
		tst_res(TFAIL | TERRNO, "mkstemp failed");
	close(fd);
	fd = open(tempname, O_WRONLY);
	TST_EXP_FAIL(readahead(fd, 0, getpagesize()), EBADF);
	close(fd);
	unlink(tempname);
}

void test_invalid_fd(void)
{
	int fd[2];
	tst_res(TINFO, "%s pipe", __func__);
	pipe(fd);
	TST_EXP_FAIL(readahead(fd[0], 0, getpagesize()), EINVAL);
	close(fd[0]);
	close(fd[1]);
	tst_res(TINFO, "%s socket", __func__);
	fd[0] = socket(AF_INET, SOCK_STREAM, 0);
	TST_EXP_FAIL(readahead(fd[0], 0, getpagesize()), EINVAL);
	close(fd[0]);
}

void test_readahead(void)
{
	test_bad_fd();
	test_invalid_fd();
}

void setup(void)
{
	tst_syscall(__NR_readahead, 0, 0, 0);
}

void main(){
	setup();
	cleanup();
}
