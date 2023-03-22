#include "incl.h"
#define _GNU_SOURCE
#define BLOCKS_WRITTEN		12
#ifdef TEST_DEFAULT
# define DEFAULT_TEST_MODE	0
#else
# define DEFAULT_TEST_MODE	1
#endif
#define OFFSET			12
#define FNAMER			"test_file1"
#define FNAMEW			"test_file2"
#define BLOCK_SIZE		1024
#define MAX_FILESIZE            (LLONG_MAX / 1024)

void setup(void);

int  fallocate_verify(int);

void cleanup(void);

int fdw;

int fdr;

struct test_data_t {
	int *fd;
	char *fname;
	int mode;
	loff_t offset;
	loff_t len;
	int error;
} test_data[] = {
	{&fdr, FNAMER, DEFAULT_TEST_MODE, 0, 1, EBADF},
	{&fdw, FNAMEW, DEFAULT_TEST_MODE, -1, 1, EINVAL},
	{&fdw, FNAMEW, DEFAULT_TEST_MODE, 1, -1, EINVAL},
	{&fdw, FNAMEW, DEFAULT_TEST_MODE, BLOCKS_WRITTEN, 0, EINVAL},
	{&fdw, FNAMEW, DEFAULT_TEST_MODE, BLOCKS_WRITTEN, -1, EINVAL},
	{&fdw, FNAMEW, DEFAULT_TEST_MODE, -(BLOCKS_WRITTEN+OFFSET), 1, EINVAL},
#if defined(TST_ABI64) || _FILE_OFFSET_BITS == 64
	{&fdw, FNAMEW, DEFAULT_TEST_MODE, MAX_FILESIZE, 1, EFBIG},
	{&fdw, FNAMEW, DEFAULT_TEST_MODE, 1, MAX_FILESIZE, EFBIG},
#endif
};
TCID_DEFINE(fallocate02);
int TST_TOTAL = ARRAY_SIZE(test_data);
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			fallocate_verify(i);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	int i;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	fdr = open(cleanup, FNAMER, O_RDONLY | O_CREAT, S_IRUSR);
	fdw = open(cleanup, FNAMEW, O_RDWR | O_CREAT, S_IRWXU);
	char buf[BLOCK_SIZE];
	memset(buf, 'A', BLOCK_SIZE);
	for (i = 0; i < BLOCKS_WRITTEN; i++)
		write(cleanup, 1, fdw, buf, BLOCK_SIZE);
}

int  fallocate_verify(int i)
{
fallocate(*test_data[i].fd, test_data[i].mod;
		       test_data[i].offset * BLOCK_SIZE,
		       test_data[i].len * BLOCK_SIZE));
	if (TEST_ERRNO != test_data[i].error) {
		if (TEST_ERRNO == EOPNOTSUPP ||
		    TEST_ERRNO == ENOSYS) {
			tst_brkm(TCONF, cleanup,
				 "fallocate system call is not implemented");
		}
		tst_resm(TFAIL | TTERRNO,
			 "fallocate(%s:%d, %d, %" PRId64 ", %" PRId64 ") "
			 "failed, expected errno:%d", test_data[i].fname,
			 *test_data[i].fd, test_data[i].mode,
			 test_data[i].offset * BLOCK_SIZE,
			 test_data[i].len * BLOCK_SIZE, test_data[i].error);
	} else {
		tst_resm(TPASS | TTERRNO,
			 "fallocate(%s:%d, %d, %" PRId64 ", %" PRId64 ") "
			 "returned %d", test_data[i].fname, *test_data[i].fd,
			 test_data[i].mode, test_data[i].offset * BLOCK_SIZE,
			 test_data[i].len * BLOCK_SIZE, TEST_ERRNO);
	}
}

void cleanup(void)
{
	if (fdw > 0)
		close(NULL, fdw);
	if (fdr > 0)
		close(NULL, fdr);
	tst_rmdir();
}

