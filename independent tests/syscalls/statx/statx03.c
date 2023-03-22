#include "incl.h"
#define _GNU_SOURCE
#define TESTFILE "test_file"
#define MODE 0644

char long_pathname[257];

char *test_fname = TESTFILE;

char *efault_fname;

char *empty_fname = "";

char *etoolong_fname = long_pathname;

struct test_case {
	uint32_t dfd;
	char **filename;
	uint32_t flag;
	uint32_t mask;
	int32_t errnum;
} tcases[] = {
	{.dfd = -1, .filename = &test_fname, .flag = 0,
	 .mask = 0, .errnum = EBADF},
	{.dfd = AT_FDCWD, .filename = &efault_fname, .flag = 0,
	 .mask = 0, .errnum = EFAULT},
	{.dfd = AT_FDCWD, .filename = &test_fname, .flag = -1,
	 .mask = 0, .errnum = EINVAL},
	{.dfd = AT_FDCWD, .filename = &test_fname, .flag = 0,
	 .mask = -1, .errnum = EINVAL},
	{.dfd = AT_FDCWD, .filename = &empty_fname, .flag = 0,
	 .mask = 0, .errnum = ENOENT},
	{.dfd = 1, .filename = &test_fname, .flag = 0,
	 .mask = 0, .errnum = ENOTDIR},
	{.dfd = AT_FDCWD, .filename = &etoolong_fname, .flag = 0,
	 .mask = 0, .errnum = ENAMETOOLONG},
};

void run_test(unsigned int i)
{
	struct statx buf;
	struct test_case *tc = &tcases[i];
statx(tc->dfd, *(tc->filename), tc->fla;
		   tc->mask, &buf));
	if (TST_RET != -1) {
		tst_res(TFAIL, "statx() returned with %ld", TST_RET);
		return;
	}
	if (tc->errnum == TST_ERR) {
		tst_res(TPASS | TTERRNO, "statx() failed with");
		return;
	}
	tst_res(TFAIL | TTERRNO,
		"statx() should fail with %s", tst_strerrno(tc->errnum));
}

void setup(void)
{
	int file_fd;
	file_fd = open(TESTFILE, O_RDWR | O_CREAT, MODE);
	close(file_fd);
	memset(long_pathname, '@', sizeof(long_pathname));
	long_pathname[sizeof(long_pathname) - 1] = 0;
	efault_fname = tst_get_bad_addr(NULL);
}

void main(){
	setup();
	cleanup();
}
