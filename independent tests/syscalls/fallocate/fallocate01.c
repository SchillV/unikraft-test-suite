#include "incl.h"
#define _GNU_SOURCE
#define BLOCKS_WRITTEN 12
void get_blocksize(int);
void populate_files(int fd);
void runtest(int, int, loff_t);
char *TCID = "fallocate01";
int fd_mode1, fd_mode2;
int TST_TOTAL = 2;
loff_t block_size;
int buf_size;
void cleanup(void)
{
	if (close(fd_mode1) == -1)
		tst_resm(TWARN | TERRNO, "close(%s) failed", fname_mode1);
	if (close(fd_mode2) == -1)
		tst_resm(TWARN | TERRNO, "close(%s) failed", fname_mode2);
	tst_rmdir();
}
void setup(void)
{
	TEST_PAUSE;
	tst_tmpdir();
	sprintf(fname_mode1, "tfile_mode1_%d", getpid());
	fd_mode1 = open(cleanup, fname_mode1, O_RDWR | O_CREAT, 0700);
	get_blocksize(fd_mode1);
	populate_files(fd_mode1);
	sprintf(fname_mode2, "tfile_mode2_%d", getpid());
	fd_mode2 = open(cleanup, fname_mode2, O_RDWR | O_CREAT, 0700);
	populate_files(fd_mode2);
}
void get_blocksize(int fd)
{
	struct stat file_stat;
	if (fstat(fd, &file_stat) < 0)
		tst_resm(TFAIL | TERRNO,
			 "fstat failed while getting block_size");
	block_size = file_stat.st_blksize;
	buf_size = block_size;
}
void populate_files(int fd)
{
	char buf[buf_size + 1];
	int index;
	int blocks;
	int data;
	for (blocks = 0; blocks < BLOCKS_WRITTEN; blocks++) {
		for (index = 0; index < buf_size; index++)
			buf[index] = 'A' + (index % 26);
		buf[buf_size] = '\0';
		if ((data = write(fd, buf, buf_size)) == -1)
			tst_brkm(TBROK | TERRNO, cleanup, "write failed");
	}
}
int main(int ac, char **av)
{
	loff_t expected_size;
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		expected_size = BLOCKS_WRITTEN * block_size + block_size;
		runtest(0, fd_mode1, expected_size);
		expected_size = BLOCKS_WRITTEN * block_size;
		runtest(FALLOC_FL_KEEP_SIZE, fd_mode2, expected_size);
	}
	cleanup();
	tst_exit();
}
void runtest(int mode, int fd, loff_t expected_size)
{
	loff_t offset;
	loff_t len = block_size;
	loff_t write_offset, lseek_offset;
	offset = lseek(fd, 0, SEEK_END);
	struct stat file_stat;
	errno = 0;
fallocate(fd, mode, offset, len);
	if (TEST_RETURN != 0) {
		if (TEST_ERRNO == EOPNOTSUPP || TEST_ERRNO == ENOSYS) {
			tst_brkm(TCONF, cleanup,
				 "fallocate system call is not implemented");
		}
		tst_resm(TFAIL | TTERRNO,
			 "fallocate(%d, %d, %" PRId64 ", %" PRId64 ") failed",
			 fd, mode, offset, len);
		return;
	} else {
		tst_resm(TPASS,
			 "fallocate(%d, %d, %" PRId64 ", %" PRId64
			 ") returned %ld", fd, mode, offset, len,
			 TEST_RETURN);
	}
	if (fstat(fd, &file_stat) < 0)
		tst_resm(TFAIL | TERRNO, "fstat failed after fallocate()");
	if (file_stat.st_size != expected_size)
		tst_resm(TFAIL | TERRNO,
			 "fstat test fails on fallocate (%d, %d, %" PRId64 ", %"
			 PRId64 ") Failed on mode", fd, mode, offset, len);
	write_offset = random() % len;
	lseek_offset = lseek(fd, write_offset, SEEK_CUR);
	if (lseek_offset != offset + write_offset) {
		tst_resm(TFAIL | TERRNO,
			 "lseek fails in fallocate(%d, %d, %" PRId64 ", %"
			 PRId64 ") failed on mode", fd, mode, offset, len);
		return;
	}
write(fd, "A", 1);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL | TTERRNO,
			 "write fails in fallocate(%d, %d, %" PRId64 ", %"
			 PRId64 ") failed", fd, mode, offset, len);
	} else {
		tst_resm(TPASS,
			 "write operation on fallocated(%d, %d, %"
			 PRId64 ", %" PRId64 ") returned %ld", fd, mode,
			 offset, len, TEST_RETURN);
	}
}

