#include "incl.h"
#define TEST_FILE	"test_file"
#define SYMLINK_FILE	"symlink_file"
#define BUFF_SIZE	256

int file_fd, dir_fd;

struct test_case_t {
	int *dirfd;
	const char *pathname;
	size_t bufsiz;
	int exp_errno;
} test_cases[] = {
	{&dir_fd, SYMLINK_FILE, 0, EINVAL},
	{&dir_fd, TEST_FILE, BUFF_SIZE, EINVAL},
	{&file_fd, SYMLINK_FILE, BUFF_SIZE, ENOTDIR},
	{&dir_fd, "test_file/test_file", BUFF_SIZE, ENOTDIR},
};
char *TCID = "readlinkat02";
int TST_TOTAL = ARRAY_SIZE(test_cases);

void setup(void);

void cleanup(void);

int  readlinkat_verify(const struct test_case_t *);
int main(int argc, char **argv)
{
	int i, lc;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			readlinkat_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	dir_fd = open(cleanup, "./", O_RDONLY);
	file_fd = open(cleanup, TEST_FILE, O_RDWR | O_CREAT, 0644);
	symlink(cleanup, TEST_FILE, SYMLINK_FILE);
}

int  readlinkat_verify(const struct test_case_t *test)
{
	char buf[BUFF_SIZE];
readlinkat(*test->dirfd, test->pathname, buf, test->bufsiz);
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL, "readlinkat succeeded unexpectedly");
		return;
	}
	if (TEST_ERRNO == test->exp_errno) {
		tst_resm(TPASS | TTERRNO, "readlinkat failed as expected");
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "readlinkat failed unexpectedly; expected: %d - %s",
			 test->exp_errno, strerror(test->exp_errno));
	}
}

void cleanup(void)
{
	close(dir_fd);
	close(file_fd);
	tst_rmdir();
}

