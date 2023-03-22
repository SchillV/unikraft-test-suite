#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE1	"testfile"
#define TEST_FILE2	"t_file/testfile"
#define TEST_FILE3	"testfile3"
#define TEST_SYM1	"testsymlink1"
#define TEST_SYM2	"testsymlink2"
#define TEST_DIR1	"testdir"
#define FILE_MODE	S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define NEW_MODE	S_IRUSR | S_IRGRP | S_IROTH
#define DIR_MODE	S_IRWXU
#define TRUNC_LEN	256
#define MAX_FSIZE	(16*1024*1024)

char long_pathname[PATH_MAX + 2];

struct test_case_t {
	char *pathname;
	off_t length;
	int exp_errno;
} test_cases[] = {
	{ TEST_FILE1, TRUNC_LEN, EACCES },
	{ TEST_FILE2, TRUNC_LEN, ENOTDIR },
	{ NULL, TRUNC_LEN, EFAULT },
	{ long_pathname, TRUNC_LEN, ENAMETOOLONG },
	{ "", TRUNC_LEN, ENOENT },
	{ TEST_DIR1, TRUNC_LEN, EISDIR },
	{ TEST_FILE3, MAX_FSIZE*2, EFBIG },
	{ TEST_SYM1, TRUNC_LEN, ELOOP }
};

void setup(void)
{
	struct passwd *ltpuser;
	struct rlimit rlim = {
		.rlim_cur = MAX_FSIZE,
		.rlim_max = MAX_FSIZE,
	};
	sigset_t signalset;
	unsigned int n;
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
	touch(TEST_FILE1, NEW_MODE, NULL);
	touch("t_file", FILE_MODE, NULL);
	memset(long_pathname, 'a', PATH_MAX + 1);
	mkdir(TEST_DIR1, DIR_MODE);
	touch(TEST_FILE3, FILE_MODE, NULL);
	symlink(TEST_SYM1, TEST_SYM2);
	symlink(TEST_SYM2, TEST_SYM1);
	setrlimit(RLIMIT_FSIZE, &rlim);
	sigemptyset(&signalset);
	sigaddset(&signalset, SIGXFSZ);
	sigprocmask(SIG_BLOCK, &signalset, NULL);
	for (n = 0; n < ARRAY_SIZE(test_cases); n++) {
		if (!test_cases[n].pathname)
			test_cases[n].pathname = tst_get_bad_addr(NULL);
	}
}

int  verify_truncate(unsigned int n)
{
	struct test_case_t *tc = &test_cases[n];
	TST_EXP_FAIL(truncate(tc->pathname, tc->length), tc->exp_errno);
}

void main(){
	setup();
	cleanup();
}
