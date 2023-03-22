#include "incl.h"
#define TESTFILE	"testfile"
#define FILE_SIZE	1024
#define TRUNC_LEN1	256
#define TRUNC_LEN2	512

int fd;

struct tcase {
	off_t trunc_len;
	off_t read_off;
	off_t read_count;
	char exp_char;
} tcases[] = {
	{TRUNC_LEN1, 0, TRUNC_LEN1, 'a'},
	{TRUNC_LEN2, TRUNC_LEN1, TRUNC_LEN1, '\0'},
};

int  verify_truncate(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct stat stat_buf;
	char read_buf[tc->read_count];
	int i;
	memset(read_buf, 'b', tc->read_count);
truncate(TESTFILE, tc->trunc_len);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "truncate(%s, %ld) failed",
			TESTFILE, tc->trunc_len);
		return;
	}
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO,
			"truncate(%s, %ld) returned invalid value %ld",
			TESTFILE, tc->trunc_len, TST_RET);
		return;
	}
	stat(TESTFILE, &stat_buf);
	if (stat_buf.st_size != tc->trunc_len) {
		tst_res(TFAIL, "%s: Incorrect file size %ld, expected %ld",
			TESTFILE, stat_buf.st_size, tc->trunc_len);
		return;
	}
	if (lseek(fd, 0, SEEK_CUR)) {
		tst_res(TFAIL, "truncate(%s, %ld) changes offset",
			TESTFILE, tc->trunc_len);
		return;
	}
	pread(1, fd, read_buf, tc->read_count, tc->read_off);
	for (i = 0; i < tc->read_count; i++) {
		if (read_buf[i] != tc->exp_char) {
			tst_res(TFAIL, "%s: wrong content %c, expected %c",
				TESTFILE, read_buf[i], tc->exp_char);
			return;
		}
	}
	tst_res(TPASS, "truncate(%s, %ld) succeeded",
		TESTFILE, tc->trunc_len);
}

void setup(void)
{
	fd = open(TESTFILE, O_RDWR | O_CREAT, 0644);
	tst_fill_fd(fd, 'a', FILE_SIZE, 1);
	lseek(fd, 0, SEEK_SET);
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
