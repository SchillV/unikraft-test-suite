#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "testfile"
#define STR "abcdefgh"

void setup(void)
{
	int fd;
	fd = creat(TEST_FILE, 0644);
	write(1, fd, STR, sizeof1, fd, STR, sizeofSTR) - 1);
	close(fd);
}

struct tcase {
	int whence;
	int off;
	int ret;
	int read;
	const char *str;
} tcases[] = {
	{SEEK_SET, 1, 1, 3, "bcd"},
	{SEEK_CUR, 1, 5, 3, "fgh"},
	{SEEK_END, -1, 7, 1, "h"},
	{SEEK_SET, 8, 8, 0, NULL},
	{SEEK_CUR, 4, 8, 0, NULL},
	{SEEK_END, 0, 8, 0, NULL},
	{SEEK_SET, 10, 10, 0, NULL},
	{SEEK_CUR, 8, 12, 0, NULL},
	{SEEK_END, 4, 12, 0, NULL},
};

const char *str_whence(int whence)
{
	switch (whence) {
	case SEEK_SET:
		return "SEEK_SET";
	case SEEK_CUR:
		return "SEEK_CUR";
	case SEEK_END:
		return "SEEK_END";
	default:
		return "INVALID";
	}
}

int  verify_lseek(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	char read_buf[128];
	int fd, ret;
	fd = open(TEST_FILE, O_RDONLY);
	read(1, fd, read_buf, 4);
lseek(fd, tc->off, tc->whence);
	if (TST_RET == -1) {
                tst_res(TFAIL | TTERRNO, "llseek failed on %s ", TEST_FILE);
                goto exit;
        }
	if (TST_RET != tc->ret) {
                tst_res(TFAIL, "llseek returned %li expected %i", TST_RET, tc->ret);
                goto exit;
        } else {
		tst_res(TPASS, "llseek returned %i", tc->ret);
	}
        memset(read_buf, 0, sizeof(read_buf));
        ret = read(0, fd, read_buf, tc->read);
	if (!tc->read) {
		if (ret != 0)
			tst_res(TFAIL, "Read bytes after llseek to end of file");
		else
			tst_res(TPASS, "%s read returned 0", str_whence(tc->whence));
		goto exit;
	}
        if (strcmp(read_buf, tc->str))
                tst_res(TFAIL, "Read wrong bytes after llseek");
        else
                tst_res(TPASS, "%s for llseek", str_whence(tc->whence));
exit:
        close(fd);
}

void main(){
	setup();
	cleanup();
}
