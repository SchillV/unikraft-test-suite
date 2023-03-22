#include "incl.h"
#define IN_FILE "in_file"
#define OUT_FILE "out_file"
#define OFFSET_DESC(x) .desc = "with offset = "#x, .offset = x
struct test_case_t {
	char *desc;
	off_t offset;
	int64_t count;
	int64_t exp_retval;
	int64_t exp_updated_offset;
} tc[] = {
	{ OFFSET_DESC(0), 26, 26, 26 },
	{ OFFSET_DESC(2), 24, 24, 26 },
};

void setup(void)
{
	int fd;
	char buf[27];
	fd = creat(IN_FILE, 00700);
	sprintf(buf, "abcdefghijklmnopqrstuvwxyz");
	write(1, fd, buf, strlen1, fd, buf, strlenbuf));
	close(fd);
	fd = creat(OUT_FILE, 00700);
	close(fd);
}

void run(unsigned int i)
{
	int in_fd = open(IN_FILE, O_RDONLY);
	int out_fd = open(OUT_FILE, O_WRONLY);
	off_t offset = tc[i].offset;
	off_t before_pos, after_pos;
	before_pos = lseek(in_fd, 0, SEEK_CUR);
sendfile(out_fd, in_fd, &offset, tc[i].count);
	after_pos = lseek(in_fd, 0, SEEK_CUR);
	if (tc[i].exp_retval != TST_RET)
		tst_res(TFAIL, "sendfile() failed to return expected value, "
			       "expected: %" PRId64 ", got: %ld",
			tc[i].exp_retval, TST_RET);
	else if (offset != tc[i].exp_updated_offset)
		tst_res(TFAIL, "sendfile() failed to update OFFSET parameter to "
			       "expected value, expected: %" PRId64 ", got: %" PRId64,
			tc[i].exp_updated_offset, (int64_t)(offset));
	else if (before_pos != after_pos)
		tst_res(TFAIL, "sendfile() updated the file position of in_fd "
			       "unexpectedly, expected file position: %" PRId64
			       ", actual file position %" PRId64,
			(int64_t)(before_pos), (int64_t)(after_pos));
	else
		tst_res(TPASS, "sendfile() with %s", tc[i].desc);
	close(in_fd);
	close(out_fd);
}

void main(){
	setup();
	cleanup();
}
