#include "incl.h"
#define _GNU_SOURCE

int fd;

size_t block_size;

size_t buf_size;
#define MNTPOINT "fallocate"
#define FNAME MNTPOINT "/fallocate.txt"
#define NUM_OF_BLOCKS	3

char *verbose;

void get_blocksize(void)
{
	struct stat file_stat;
	fstat(fd, &file_stat);
	block_size = file_stat.st_blksize;
	buf_size = NUM_OF_BLOCKS * block_size;
}

size_t get_allocsize(void)
{
	struct stat file_stat;
	fsync(fd);
	fstat(fd, &file_stat);
	return file_stat.st_blocks * 512;
}

void fill_tst_buf(char buf[])
{
	int i;
	for (i = 0; i < NUM_OF_BLOCKS; ++i)
		memset(buf + i * block_size, 'a' + i, block_size);
}

void check_file_data(const char exp_buf[], size_t size)
{
	char rbuf[size];
	tst_res(TINFO, "reading the file, compare with expected buffer");
	lseek(fd, 0, SEEK_SET);
	read(1, fd, rbuf, size);
	if (memcmp(exp_buf, rbuf, size)) {
		if (verbose) {
			tst_res_hexd(TINFO, exp_buf, size, "expected:");
			tst_res_hexd(TINFO, rbuf, size, "but read:");
		}
		tst_brk(TFAIL, "not expected file data");
	}
}

void test01(void)
{
	tst_res(TINFO, "allocate '%zu' bytes", buf_size);
	if (fallocate(fd, 0, 0, buf_size) == -1) {
		if (errno == ENOSYS || errno == EOPNOTSUPP)
			tst_brk(TCONF, "fallocate() not supported");
		tst_brk(TFAIL | TERRNO, "fallocate() failed");
	}
	char buf[buf_size];
	fill_tst_buf(buf);
	write(1, fd, buf, buf_size);
	tst_res(TPASS, "test-case succeeded");
}

void test02(void)
{
	size_t alloc_size0 = get_allocsize();
	tst_res(TINFO, "read allocated file size '%zu'", alloc_size0);
	tst_res(TINFO, "make a hole with FALLOC_FL_PUNCH_HOLE");
	if (fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE,
	    block_size, block_size) == -1) {
		if (errno == EOPNOTSUPP) {
			tst_brk(TCONF,
			        "FALLOC_FL_PUNCH_HOLE not supported");
		}
		tst_brk(TFAIL | TERRNO, "fallocate() failed");
	}
	tst_res(TINFO, "check that file has a hole with lseek(,,SEEK_HOLE)");
	off_t ret = lseek(fd, 0, SEEK_HOLE);
	if (ret != (ssize_t)block_size) {
		if (errno != EINVAL) {
			tst_brk(TFAIL | TERRNO,
				 "fallocate() or lseek() failed");
		}
		tst_brk(TBROK | TERRNO,
			"lseek() doesn't support SEEK_HOLE");
	} else {
		tst_res(TINFO, "found a hole at '%ld' offset", ret);
	}
	size_t alloc_size1 = get_allocsize();
	tst_res(TINFO, "allocated file size before '%zu' and after '%zu'",
		 alloc_size0, alloc_size1);
	if ((alloc_size0 - block_size) != alloc_size1)
		tst_brk(TFAIL, "not expected allocated size");
	char exp_buf[buf_size];
	fill_tst_buf(exp_buf);
	memset(exp_buf + block_size, 0, block_size);
	check_file_data(exp_buf, buf_size);
	tst_res(TPASS, "test-case succeeded");
}

void test03(void)
{
	tst_res(TINFO, "zeroing file space with FALLOC_FL_ZERO_RANGE");
	if (tst_kvercmp(3, 15, 0) < 0) {
		tst_brk(TCONF,
			"FALLOC_FL_ZERO_RANGE needs Linux 3.15 or newer");
	}
	size_t alloc_size0 = get_allocsize();
	tst_res(TINFO, "read current allocated file size '%zu'", alloc_size0);
	if (fallocate(fd, FALLOC_FL_ZERO_RANGE, block_size - 1,
	    block_size + 2) == -1) {
		if (errno == EOPNOTSUPP) {
			tst_brk(TCONF,
			        "FALLOC_FL_ZERO_RANGE not supported");
		}
		tst_brk(TFAIL | TERRNO, "fallocate failed");
	}
	 * filled with zeros. Check it.
	 */
	size_t alloc_size1 = get_allocsize();
	tst_res(TINFO, "allocated file size before '%zu' and after '%zu'",
		 alloc_size0, alloc_size1);
	if ((alloc_size0 + block_size) != alloc_size1)
		tst_brk(TFAIL, "not expected allocated size");
	char exp_buf[buf_size];
	fill_tst_buf(exp_buf);
	memset(exp_buf + block_size - 1, 0, block_size + 2);
	check_file_data(exp_buf, buf_size);
	tst_res(TPASS, "test-case succeeded");
}

void test04(void)
{
	tst_res(TINFO, "collapsing file space with FALLOC_FL_COLLAPSE_RANGE");
	size_t alloc_size0 = get_allocsize();
	tst_res(TINFO, "read current allocated file size '%zu'", alloc_size0);
	if (fallocate(fd, FALLOC_FL_COLLAPSE_RANGE, block_size,
	    block_size) == -1) {
		if (errno == EOPNOTSUPP) {
			tst_brk(TCONF,
			        "FALLOC_FL_COLLAPSE_RANGE not supported");
		}
		tst_brk(TFAIL | TERRNO, "fallocate failed");
	}
	size_t alloc_size1 = get_allocsize();
	tst_res(TINFO, "allocated file size before '%zu' and after '%zu'",
		 alloc_size0, alloc_size1);
	if ((alloc_size0 - block_size) != alloc_size1)
		tst_brk(TFAIL, "not expected allocated size");
	size_t size = buf_size - block_size;
	char tmp_buf[buf_size];
	char exp_buf[size];
	fill_tst_buf(tmp_buf);
	memcpy(exp_buf, tmp_buf, block_size);
	memcpy(exp_buf + block_size, tmp_buf + 2 * block_size,
	       buf_size - block_size * 2);
	exp_buf[block_size - 1] = exp_buf[block_size] = '\0';
	check_file_data(exp_buf, size);
	tst_res(TPASS, "test-case succeeded");
}

void test05(void)
{
	tst_res(TINFO, "inserting space with FALLOC_FL_INSERT_RANGE");
	size_t alloc_size0 = get_allocsize();
	tst_res(TINFO, "read current allocated file size '%zu'", alloc_size0);
	if (fallocate(fd, FALLOC_FL_INSERT_RANGE, block_size,
	    block_size) == -1) {
		if (errno == EOPNOTSUPP) {
			tst_brk(TCONF,
				"FALLOC_FL_INSERT_RANGE not supported");
		}
		tst_brk(TFAIL | TERRNO, "fallocate failed");
	}
	if (fallocate(fd, FALLOC_FL_ZERO_RANGE, block_size, block_size) == -1)
		tst_brk(TFAIL | TERRNO, "fallocate failed");
	size_t alloc_size1 = get_allocsize();
	tst_res(TINFO, "allocated file size before '%zu' and after '%zu'",
		 alloc_size0, alloc_size1);
	if ((alloc_size0 + block_size) != alloc_size1)
		tst_brk(TFAIL, "not expected allocated size");
	char exp_buf[buf_size];
	fill_tst_buf(exp_buf);
	memset(exp_buf + block_size - 1, 0, block_size + 2);
	check_file_data(exp_buf, buf_size);
	tst_res(TPASS, "test-case succeeded");
}

void (*tcases[])(void) = {
	test01, test02, test03, test04, test05
};

void run(unsigned int i)
{
	tcases[i]();
}

void setup(void)
{
	fd = open(FNAME, O_RDWR | O_CREAT, 0700);
	get_blocksize();
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
