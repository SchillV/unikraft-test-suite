#include "incl.h"
#define IN_FILE "in_file"
#define OUT_FILE "out_file"

struct stat sb;

void setup(void)
{
	int fd;
	char buf[27];
	fd = creat(IN_FILE, 00700);
	sprintf(buf, "abcdefghijklmnopqrstuvwxyz");
	write(1, fd, buf, strlen1, fd, buf, strlenbuf));
	fstat(fd, &sb);
	close(fd);
	fd = creat(OUT_FILE, 00700);
	close(fd);
}

void run(void)
{
	off_t after_pos;
	int in_fd = open(IN_FILE, O_RDONLY);
	int out_fd = open(OUT_FILE, O_WRONLY);
sendfile(out_fd, in_fd, NULL, sb.st_size);
	after_pos = lseek(in_fd, 0, SEEK_CUR);
	if (sb.st_size != TST_RET)
		tst_res(TFAIL, "sendfile() failed to return expected value, expected: %"
			PRId64 ", got: %ld",
			sb.st_size, TST_RET);
	else if (after_pos != sb.st_size)
		tst_res(TFAIL, "sendfile() updated the file position of in_fd unexpectedly,"
			" expected file position: %" PRId64
			" actual file position %" PRId64,
			(int64_t)(sb.st_size), (int64_t)(after_pos));
	else
		tst_res(TPASS, "sendfile() with offset=NULL");
	close(in_fd);
	close(out_fd);
}

void main(){
	setup();
	cleanup();
}
