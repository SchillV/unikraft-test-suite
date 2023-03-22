#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT "mntpoint"
#define FALLOCATE_BLOCKS 256
#define DEALLOCATE_BLOCKS 64
#define TESTED_FLAGS "fallocate(FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE)"

char *buf;

blksize_t blocksize;

long bufsize;

void setup(void)
{
	int fd;
	struct stat statbuf;
	fd = open(MNTPOINT "/test_file", O_WRONLY | O_CREAT, 0644);
	 * Use real FS block size, otherwise fallocate() call will test
	 * different things on different platforms
	 */
	fstat(fd, &statbuf);
	blocksize = statbuf.st_blksize;
	bufsize = FALLOCATE_BLOCKS * blocksize;
	buf = malloc(bufsize);
	close(fd);
}

void run(void)
{
	int fd;
	long extsize, tmp;
	fd = open(MNTPOINT "/test_file", O_WRONLY | O_CREAT | O_TRUNC,
		0644);
fallocate(fd, 0, 0, bufsize);
	if (TST_RET) {
		if (TST_ERR == ENOTSUP)
			tst_brk(TCONF | TTERRNO, "fallocate() not supported");
		tst_brk(TBROK | TTERRNO, "fallocate(fd, 0, 0, %ld)", bufsize);
	}
	tst_fill_fs(MNTPOINT, 1, TST_FILL_RANDOM);
write(fd, buf, bufsize);
	if (TST_RET < 0)
		tst_res(TFAIL | TTERRNO, "write() failed unexpectedly");
	else if (TST_RET != bufsize)
		tst_res(TFAIL, "Short write(): %ld bytes (expected %zu)",
			TST_RET, bufsize);
	else
		tst_res(TPASS, "write() wrote %ld bytes", TST_RET);
	 * Some file systems may still have a few extra blocks that can be
	 * allocated.
	 */
	for (TST_RET = 0, extsize = 0; !TST_RET; extsize += blocksize)
fallocate(fd, 0, bufsize + extsize, blocksize);
	if (TST_RET != -1) {
		tst_res(TFAIL, "Invalid fallocate() return value %ld", TST_RET);
		return;
	}
	if (TST_ERR != ENOSPC) {
		tst_res(TFAIL | TTERRNO, "fallocate() should fail with ENOSPC");
		return;
	}
	extsize -= blocksize;
	tst_res(TINFO, "fallocate()d %ld extra blocks on full FS",
		extsize / blocksize);
	for (tmp = extsize; tmp > 0; tmp -= TST_RET) {
write(fd, buf, MIN(bufsize, tmp));
		if (TST_RET <= 0) {
			tst_res(TFAIL | TTERRNO, "write() failed unexpectedly");
			return;
		}
	}
	tst_res(TPASS, "fallocate() on full FS");
	if (!strcmp(tst_device->fs_type, "btrfs"))
		tmp = bufsize + extsize;
	else
		tmp = DEALLOCATE_BLOCKS * blocksize;
fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, 0, tmp);
	if (TST_RET == -1) {
		if (TST_ERR == ENOTSUP)
			tst_brk(TCONF, TESTED_FLAGS);
		tst_brk(TBROK | TTERRNO, TESTED_FLAGS);
	}
	tst_res(TPASS, TESTED_FLAGS);
write(fd, buf, 10);
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "write()");
	else
		tst_res(TPASS, "write()");
	close(fd);
	tst_purge_dir(MNTPOINT);
}

void cleanup(void)
{
	free(buf);
}

void main(){
	setup();
	cleanup();
}
