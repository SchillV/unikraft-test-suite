#include "incl.h"
#define _GNU_SOURCE
#define UNIT_COUNT   3
#define UNIT_BLOCKS  10
#define FILE_BLOCKS  (UNIT_BLOCKS * UNIT_COUNT)

int fd;

blksize_t block_size;

struct tparam {
	off_t  startblock;
	off_t  offset;
	int    whence;
	char   *data;
	size_t count;
} tparams[] = {
};

void cleanup(void)
{
	close(fd);
}

void get_blocksize(void)
{
	off_t pos = 0, offset = 128;
	int shift;
	struct stat st;
	fstat(fd, &st);
	while (pos == 0 && offset < (st.st_blksize * 2)) {
		offset <<= 1;
		ftruncate(fd, 0);
		pwrite(1, fd, "a", 1, offset);
		fsync(fd);
		pos = lseek(fd, 0, SEEK_DATA);
		if (pos == -1) {
			if (errno == EINVAL || errno == EOPNOTSUPP) {
				tst_brk(TCONF | TERRNO, "SEEK_DATA "
					"and SEEK_HOLE not implemented");
			}
			tst_brk(TBROK | TERRNO, "SEEK_DATA failed");
		}
	}
	shift = offset >> 2;
	while (shift && offset < (st.st_blksize * 2)) {
		ftruncate(fd, 0);
		pwrite(1, fd, "a", 1, offset);
		fsync(fd);
		pos = lseek(fd, 0, SEEK_DATA);
		offset += pos ? -shift : shift;
		shift >>= 1;
	}
	if (!shift)
		offset += pos ? 0 : 1;
	block_size = offset;
	 * Due to some filesystems use generic_file_llseek(), e.g: CIFS,
	 * it thinks the entire file is data, only a virtual hole at the end
	 * of the file. This case can't test this situation, so if the minimum
	 * alloc size we got bigger then st.st_blksize, we think it's not
	 * a valid value.
	 */
	if (block_size > st.st_blksize) {
		tst_brk(TCONF,
		        "filesystem maybe use generic_file_llseek(), not support real SEEK_DATA/SEEK_HOLE");
	}
}

void write_data(int fd, int num)
{
	char buf[64];
	sprintf(buf, "data%02dsuffix", num);
	write(1, fd, buf, strlen1, fd, buf, strlenbuf));
}

void setup(void)
{
	int i;
	off_t offset = 0;
	char fname[255];
	sprintf(fname, "tfile_lseek_%d", getpid());
	fd = open(fname, O_RDWR | O_CREAT, 0666);
	get_blocksize();
	tst_res(TINFO, "The block size is %lld", (long long int)block_size);
	 * truncate to the expected file size directly, to keep away the effect
	 * of speculative preallocation of some filesystems (e.g. XFS)
	 */
	ftruncate(fd, FILE_BLOCKS * block_size);
	lseek(fd, 0, SEEK_HOLE);
	for (i = 0; i < UNIT_COUNT; i++) {
		offset = UNIT_BLOCKS * block_size * i;
		lseek(fd, offset, SEEK_SET);
		write_data(fd, i + 1);
	}
	lseek(fd, -128, SEEK_END);
	write_data(fd, i + 1);
	fsync(fd);
	lseek(fd, 0, SEEK_SET);
}

void test_lseek(unsigned int n)
{
	struct tparam *tp = &tparams[n];
	off_t offset;
	char buf[1024];
	int rc = 0;
	memset(buf, 0, sizeof(buf));
	offset = (tp->startblock * block_size) + tp->offset;
	offset = lseek(fd, offset, tp->whence);
	if (tp->data) {
		read(1, fd, buf, tp->count);
		rc = strcmp(buf, tp->data);
	} else {
		if (offset != lseek(fd, 0, SEEK_END))
			rc = 1;
	}
	if (rc != 0) {
		tst_res(TFAIL,
		        "The %uth test failed: %s from startblock %lld offset %lld, expect \'%s\' return \'%s\'",
		        n, (tp->whence == SEEK_DATA) ? "SEEK_DATA" : "SEEK_HOLE",
		        (long long int)tp->startblock, (long long int)tp->offset,
		        tp->data ? tp->data : "", buf);
	} else {
		tst_res(TPASS,
		        "The %uth test passed: %s from startblock %lld offset %lld",
		        n, (tp->whence == SEEK_DATA) ? "SEEK_DATA" : "SEEK_HOLE",
		        (long long int)tp->startblock, (long long int)tp->offset);
	}
}

void main(){
	setup();
	cleanup();
}
