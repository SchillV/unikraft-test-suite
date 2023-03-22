#include "incl.h"
#define TEST_FILE		"statfs_file"
#define TEST_FILE1		TEST_FILE"/statfs_file1"
#define TEST_NOEXIST		"statfs_noexist"
#define TEST_SYMLINK		"statfs_symlink"

int fd;

char test_toolong[PATH_MAX+2];

struct statfs buf;

struct test_case_t {
	char *path;
	struct statfs *buf;
	int exp_error;
} tests[] = {
	{TEST_FILE1, &buf, ENOTDIR},
	{TEST_NOEXIST, &buf, ENOENT},
	{test_toolong, &buf, ENAMETOOLONG},
	{(char *)-1, &buf, EFAULT},
	{TEST_FILE, (struct statfs *)-1, EFAULT},
	{TEST_SYMLINK, &buf, ELOOP},
};

int  statfs_verify(unsigned int n)
{
	int pid, status;
	pid = fork();
	if (!pid) {
		TST_EXP_FAIL(statfs(tests[n].path, tests[n].buf), tests[n].exp_error, "statfs()");
		exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		return;
	if (tests[n].exp_error == EFAULT &&
	    WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV) {
		tst_res(TPASS, "Got SIGSEGV instead of EFAULT");
		return;
	}
	tst_res(TFAIL, "Child %s", tst_strstatus(status));
}

void setup(void)
{
	unsigned int i;
	fd = creat(TEST_FILE, 0444);
	memset(test_toolong, 'a', PATH_MAX+1);
	for (i = 0; i < ARRAY_SIZE(tests); i++) {
		if (tests[i].path == (char *)-1)
			tests[i].path = tst_get_bad_addr(NULL);
	}
	symlink(TEST_SYMLINK, "statfs_symlink_2");
	symlink("statfs_symlink_2", TEST_SYMLINK);
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
