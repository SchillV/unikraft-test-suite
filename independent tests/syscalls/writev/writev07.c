#include "incl.h"
#define TESTFILE "testfile"
#define CHUNK 64
#define BUFSIZE (CHUNK * 4)

void *bad_addr;

void test_partially_valid_iovec(int initial_file_offset)
{
	int i, fd;
	unsigned char buffer[BUFSIZE], fpattern[BUFSIZE], tmp[BUFSIZE];
	long off_after;
	struct iovec wr_iovec[] = {
		{ buffer, CHUNK },
		{ bad_addr, CHUNK },
		{ buffer + CHUNK, CHUNK },
		{ buffer + CHUNK * 2, CHUNK },
	};
	tst_res(TINFO, "starting test with initial file offset: %d ",
		initial_file_offset);
	for (i = 0; i < BUFSIZE; i++)
		buffer[i] = i % (CHUNK - 1);
	memset(fpattern, 0xff, BUFSIZE);
	tst_fill_file(TESTFILE, 0xff, CHUNK, BUFSIZE / CHUNK);
	fd = open(TESTFILE, O_RDWR, 0644);
	lseek(fd, initial_file_offset, SEEK_SET);
writev(fd, wr_iovec, ARRAY_SIZE(wr_iovec));
	off_after = (long) lseek(fd, 0, SEEK_CUR);
	if (TST_RET == -1 && TST_ERR != EFAULT) {
		tst_res(TFAIL | TTERRNO, "unexpected errno");
		close(fd);
		return;
	}
	if (TST_RET == -1 && TST_ERR == EFAULT) {
		tst_res(TINFO, "got EFAULT");
		lseek(fd, 0, SEEK_SET);
		read(1, fd, tmp, BUFSIZE);
		if (memcmp(tmp, fpattern, BUFSIZE))
			tst_res(TFAIL, "file was written to");
		else
			tst_res(TPASS, "file stayed untouched");
		if (off_after == initial_file_offset)
			tst_res(TPASS, "offset stayed unchanged");
		else
			tst_res(TFAIL, "offset changed to %ld",
				off_after);
		close(fd);
		return;
	}
	tst_res(TINFO, "writev() has written %ld bytes", TST_RET);
	if (TST_RET > (long) wr_iovec[0].iov_len) {
		tst_res(TFAIL, "writev wrote more than expected");
		close(fd);
		return;
	}
	lseek(fd, initial_file_offset, SEEK_SET);
	read(1, fd, tmp, TST_RET);
	if (memcmp(tmp, wr_iovec[0].iov_base, TST_RET) == 0) {
		tst_res(TPASS, "file has expected content");
	} else {
		tst_res(TFAIL, "file has unexpected content");
		tst_res_hexd(TFAIL, wr_iovec[0].iov_base, TST_RET,
				"expected:");
		tst_res_hexd(TFAIL, tmp, TST_RET,
				"actual file content:");
	}
	if (off_after == initial_file_offset + TST_RET)
		tst_res(TPASS, "offset at %ld as expected", off_after);
	else
		tst_res(TFAIL, "offset unexpected %ld", off_after);
	close(fd);
}

void test_writev(void)
{
	test_partially_valid_iovec(0);
	test_partially_valid_iovec(CHUNK + 1);
	test_partially_valid_iovec(getpagesize());
	test_partially_valid_iovec(getpagesize() + 1);
}

void setup(void)
{
	bad_addr = mmap(0, 1, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS,
			0, 0);
}

