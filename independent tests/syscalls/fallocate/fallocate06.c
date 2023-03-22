#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT "mntpoint"
#define TEMPFILE MNTPOINT "/test_file"
#define WRITE_BLOCKS 8
#define FALLOCATE_BLOCKS 2
#define DEALLOCATE_BLOCKS 3
#define TESTED_FLAGS "fallocate(FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE)"
const struct test_case {
	int no_cow, fill_fs;
} testcase_list[] = {
	{1, 0},
	{1, 1},
	{0, 0},
	{0, 1}
};

int cow_support;

char *wbuf, *rbuf;

blksize_t blocksize;

long wbuf_size, rbuf_size, block_offset;

int toggle_cow(int fd, int enable)
{
	int ret, attr;
	ret = ioctl(fd, FS_IOC_GETFLAGS, &attr);
	if (ret)
		return ret;
	if (enable)
		attr &= ~FS_NOCOW_FL;
	else
		attr |= FS_NOCOW_FL;
	return ioctl(fd, FS_IOC_SETFLAGS, &attr);
}

void setup(void)
{
	unsigned char ch;
	long i;
	int fd;
	struct stat statbuf;
	fd = open(TEMPFILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	 * Set FS_NOCOW_FL flag on the temp file. Non-CoW filesystems will
	 * return error.
	 */
toggle_cow(fd, 0);
	fstat(fd, &statbuf);
	blocksize = statbuf.st_blksize;
	block_offset = MIN(blocksize / 2, (blksize_t)512);
	wbuf_size = MAX(WRITE_BLOCKS, FALLOCATE_BLOCKS) * blocksize;
	rbuf_size = (DEALLOCATE_BLOCKS + 1) * blocksize;
	close(fd);
	unlink(TEMPFILE);
	if (blocksize < 2)
		tst_brk(TCONF, "Block size %ld too small for test", blocksize);
	if (!TST_RET) {
		cow_support = 1;
	} else {
		switch (TST_ERR) {
		case ENOTSUP:
		case ENOTTY:
		case EINVAL:
		case ENOSYS:
			cow_support = 0;
			break;
		default:
			tst_brk(TBROK|TTERRNO,
				"Error checking copy-on-write support");
			break;
		}
	}
	tst_res(TINFO, "Copy-on-write is%s supported",
		cow_support ? "" : " not");
	wbuf = malloc(wbuf_size);
	rbuf = malloc(rbuf_size);
	for (i = 0, ch = 1; i < wbuf_size; i++, ch++)
		wbuf[i] = ch;
}

int check_result(const struct test_case *tc, const char *func, long exp)
{
	if (tc->fill_fs && !tc->no_cow && TST_RET < 0) {
		if (TST_RET != -1) {
			tst_res(TFAIL, "%s returned unexpected value %ld",
				func, TST_RET);
			return 0;
		}
		if (TST_ERR != ENOSPC) {
			tst_res(TFAIL | TTERRNO, "%s should fail with ENOSPC",
				func);
			return 0;
		}
		tst_res(TPASS | TTERRNO, "%s on full FS with CoW", func);
		return 1;
	}
	if (TST_RET < 0) {
		tst_res(TFAIL | TTERRNO, "%s failed unexpectedly", func);
		return 0;
	}
	if (TST_RET != exp) {
		tst_res(TFAIL,
			"Unexpected return value from %s: %ld (expected %ld)",
			func, TST_RET, exp);
		return 0;
	}
	tst_res(TPASS, "%s successful", func);
	return 1;
}

void run(unsigned int n)
{
	int fd, i, err;
	long offset, size;
	const struct test_case *tc = testcase_list + n;
	tst_res(TINFO, "Case %u. Fill FS: %s; Use copy on write: %s", n+1,
		tc->fill_fs ? "yes" : "no", tc->no_cow ? "no" : "yes");
	fd = open(TEMPFILE, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (cow_support)
		toggle_cow(fd, !tc->no_cow);
	else if (!tc->no_cow)
		tst_brk(TCONF, "File system does not support copy-on-write");
	size = WRITE_BLOCKS * blocksize;
	write(1, fd, wbuf, size);
	offset = size + block_offset;
	size = FALLOCATE_BLOCKS * blocksize;
fallocate(fd, 0, offset, size);
	if (TST_RET) {
		close(fd);
		if (TST_ERR == ENOTSUP)
			tst_brk(TCONF | TTERRNO, "fallocate() not supported");
		tst_brk(TBROK | TTERRNO, "fallocate(fd, 0, %ld, %ld)", offset,
			size);
	}
	if (tc->fill_fs)
		tst_fill_fs(MNTPOINT, 1, TST_FILL_RANDOM);
	lseek(fd, offset, SEEK_SET);
write(fd, wbuf, size);
	if (check_result(tc, "write()", size))
		tst_res(TPASS, "Misaligned allocation works as expected");
	size = DEALLOCATE_BLOCKS * blocksize;
	offset = block_offset;
fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, offse;
		size));
	if (TST_RET == -1 && TST_ERR == ENOTSUP) {
		tst_res(TCONF | TTERRNO, TESTED_FLAGS);
		goto end;
	}
	if (!check_result(tc, TESTED_FLAGS, 0) || TST_RET)
		goto end;
	lseek(fd, 0, SEEK_SET);
	read(1, fd, rbuf, rbuf_size);
	for (err = 0, i = offset; i < offset + size; i++) {
		if (rbuf[i]) {
			err = 1;
			break;
		}
	}
	err = err || memcmp(rbuf, wbuf, offset);
	offset += size;
	size = rbuf_size - offset;
	err = err || memcmp(rbuf + offset, wbuf + offset, size);
	if (err)
		tst_res(TFAIL, TESTED_FLAGS
			" did not clear the correct file range.");
	else
		tst_res(TPASS, TESTED_FLAGS " cleared the correct file range");
end:
	close(fd);
	tst_purge_dir(MNTPOINT);
}

void cleanup(void)
{
	free(wbuf);
	free(rbuf);
}

void main(){
	setup();
	cleanup();
}
