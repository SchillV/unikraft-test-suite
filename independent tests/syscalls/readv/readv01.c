#include "incl.h"
#define	CHUNK		64

char buf[CHUNK];

struct iovec *rd_iovec, *big_iovec, *multi_iovec, *lockup_iovec;

int fd;

struct testcase {
	struct iovec **iov;
	int iov_count, exp_ret;
	const char *name;
} testcase_list[] = {
	{&rd_iovec, 0, 0, "readv() with 0 I/O vectors"},
	{&rd_iovec, 3, CHUNK, "readv() with NULL I/O vectors"},
	{&big_iovec, 2, CHUNK, "readv() with too big I/O vectors"},
	{&multi_iovec, 2, 3*CHUNK/4, "readv() with multiple I/O vectors"},
	{&lockup_iovec, 2, CHUNK, "readv() with zero-len buffer"},
};

void test_readv(unsigned int n)
{
	int i, fpos, fail = 0;
	size_t j;
	char *ptr;
	const struct testcase *tc = testcase_list + n;
	struct iovec *vec;
	lseek(fd, 0, SEEK_SET);
	vec = *tc->iov;
	for (i = 0; i < tc->iov_count; i++) {
		if (vec[i].iov_base && vec[i].iov_len)
			memset(vec[i].iov_base, 0, vec[i].iov_len);
	}
readv(fd, vec, tc->iov_count);
	if (TST_RET == -1)
		tst_res(TFAIL | TTERRNO, "readv() failed unexpectedly");
	else if (TST_RET < 0)
		tst_res(TFAIL | TTERRNO, "readv() returned invalid value");
	else if (TST_RET != tc->exp_ret)
		tst_res(TFAIL, "readv() returned unexpected value %ld",
			TST_RET);
	if (TST_RET != tc->exp_ret)
		return;
	tst_res(TPASS, "%s", tc->name);
	for (i = 0, fpos = 0; i < tc->iov_count; i++) {
		ptr = vec[i].iov_base;
		for (j = 0; j < vec[i].iov_len; j++, fpos++) {
			if (ptr[j] != (fpos < tc->exp_ret ? 0x42 : 0))
				fail++;
		}
	}
	if (fail)
		tst_res(TFAIL, "Wrong buffer content");
	else
		tst_res(TPASS, "readv() correctly read %d bytes ", tc->exp_ret);
}

void setup(void)
{
	lockup_iovec[0].iov_base = tst_get_bad_addr(NULL);
	memset(buf, 0x42, sizeof(buf));
	fd = open("data_file", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	write(1, fd, buf, sizeof1, fd, buf, sizeofbuf));
	close(fd);
	fd = open("data_file", O_RDONLY);
}

void cleanup(void)
{
	if (fd >= 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
