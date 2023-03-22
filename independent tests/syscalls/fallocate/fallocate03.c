#include "incl.h"
#define _GNU_SOURCE
#define BLOCKS_WRITTEN 12
#define HOLE_SIZE_IN_BLOCKS 12
#define DEFAULT_MODE 0
#define TRUE 0
void get_blocksize(int);
void populate_file();
void file_seek(off_t);
char *TCID = "fallocate03";
char fname[255];
int fd;
struct test_data_t {
	int mode;
	loff_t offset;
	loff_t len;
	int error;
} test_data[] = {
	{
	DEFAULT_MODE, 2, 1, TRUE}, {
	DEFAULT_MODE, BLOCKS_WRITTEN, 1, TRUE}, {
	DEFAULT_MODE, BLOCKS_WRITTEN + HOLE_SIZE_IN_BLOCKS / 2 - 1, 1, TRUE},
	{
	DEFAULT_MODE, BLOCKS_WRITTEN + HOLE_SIZE_IN_BLOCKS + 1, 1, TRUE}, {
	FALLOC_FL_KEEP_SIZE, 2, 1, TRUE}, {
	FALLOC_FL_KEEP_SIZE, BLOCKS_WRITTEN, 1, TRUE}, {
	FALLOC_FL_KEEP_SIZE,
		    BLOCKS_WRITTEN + HOLE_SIZE_IN_BLOCKS / 2 + 1, 1, TRUE}, {
	FALLOC_FL_KEEP_SIZE, BLOCKS_WRITTEN + HOLE_SIZE_IN_BLOCKS + 2,
		    1, TRUE}
};
int TST_TOTAL = sizeof(test_data) / sizeof(test_data[0]);
int block_size;
int buf_size;
void cleanup(void)
{
	if (close(fd) == -1)
		tst_resm(TWARN | TERRNO, "close(%s) failed", fname);
	tst_rmdir();
}
void setup(void)
{
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(fname, "tfile_sparse_%d", getpid());
	fd = open(cleanup, fname, O_RDWR | O_CREAT, 0700);
	get_blocksize(fd);
	populate_file();
	populate_file();
}
void get_blocksize(int fd)
{
	struct stat file_stat;
	if (fstat(fd, &file_stat) < 0)
		tst_resm(TFAIL | TERRNO,
			 "fstat failed while getting block_size");
	block_size = (int)file_stat.st_blksize;
	buf_size = block_size;
}
void file_seek(off_t offset)
{
	offset *= block_size;
	lseek(fd, offset, SEEK_SET);
}
void populate_file(void)
{
	char buf[buf_size + 1];
	int index;
	int blocks;
	int data;
	for (blocks = 0; blocks < BLOCKS_WRITTEN; blocks++) {
		for (index = 0; index < buf_size; index++)
			buf[index] = 'A' + (index % 26);
		buf[buf_size] = '\0';
		if ((data = write(fd, buf, buf_size)) < 0)
			tst_brkm(TBROK | TERRNO, cleanup,
				 "Unable to write to %s", fname);
	}
}
int main(int ac, char **av)
{
	int test_index = 0;
	int lc;
	 * parse standard options
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (test_index = 0; test_index < TST_TOTAL; test_index++) {
falloca;
			     (fd, test_data[test_index].mode,
			      test_data[test_index].offset * block_size,
			      test_data[test_index].len * block_size));
			if (TEST_RETURN != test_data[test_index].error) {
				if (TEST_ERRNO == EOPNOTSUPP
				    || TEST_ERRNO == ENOSYS) {
					tst_brkm(TCONF, cleanup,
						 "fallocate system call is not implemented");
				}
				tst_resm(TFAIL | TTERRNO,
					 "fallocate(%s, %d, %" PRId64 ", %"
					 PRId64 ") failed", fname,
					 test_data[test_index].mode,
					 test_data[test_index].offset *
					 block_size,
					 test_data[test_index].len *
					 block_size);
			} else {
				tst_resm(TPASS,
					 "fallocate(%s, %d, %" PRId64
					 ", %" PRId64 ") returned %ld",
					 fname,
					 test_data[test_index].mode,
					 test_data[test_index].offset *
					 block_size,
					 test_data[test_index].len *
					 block_size, TEST_RETURN);
			}
		}
	}
	cleanup();
	tst_exit();
}

