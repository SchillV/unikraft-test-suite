#include "incl.h"
#define TESTFILE	"testfile"
#define TRUNC_LEN1	256
#define TRUNC_LEN2	512
#define FILE_SIZE	1024

int fd;

void check_and_report(off_t offset, char data, off_t trunc_len)
{
	int i, file_length;
	char buf[FILE_SIZE];
	struct stat stat_buf;
	memset(buf, '*', sizeof(buf));
	fstat(fd, &stat_buf);
	file_length = stat_buf.st_size;
	if (file_length != trunc_len) {
		tst_res(TFAIL, "ftruncate() got incorrected size: %d",
			file_length);
		return;
	}
	lseek(fd, offset, SEEK_SET);
	read(0, fd, buf, sizeof0, fd, buf, sizeofbuf));
	for (i = 0; i < TRUNC_LEN1; i++) {
		if (buf[i] != data) {
			tst_res(TFAIL,
				"ftruncate() got incorrect data %i, expected %i",
				buf[i], data);
			return;
		}
	}
	tst_res(TPASS, "ftruncate() succeeded");
}

int  verify_ftruncate(
{
	tst_res(TINFO, "Truncated length smaller than file size");
ftruncate(fd, TRUNC_LEN1);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "ftruncate() failed");
		return;
	}
	check_and_report(0, 'a', TRUNC_LEN1);
	tst_res(TINFO, "Truncated length exceeds file size");
ftruncate(fd, TRUNC_LEN2);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "ftruncate() failed");
		return;
	}
	check_and_report(TRUNC_LEN1, 0, TRUNC_LEN2);
}

void setup(void)
{
	if (tst_fill_file(TESTFILE, 'a', FILE_SIZE, 1))
		tst_brk(TBROK, "Failed to create test file");
	fd = open(TESTFILE, O_RDWR);
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
