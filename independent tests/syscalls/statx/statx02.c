#include "incl.h"
#define _GNU_SOURCE
#define TESTFILE "test_temp"
#define LINK_FILE "test_temp_ln"
#define MODE 0644
#define SIZE 14

int file_fd;

void test_empty_path(void)
{
	struct statx buf;
statx(file_fd, "", AT_EMPTY_PATH, 0, &buf);
	if (TST_RET == 0)
		tst_res(TPASS,
			"statx(file_fd, \"\", AT_EMPTY_PATH, 0, &buf)");
	else
		tst_brk(TFAIL | TTERRNO,
			"statx(file_fd, \"\", AT_EMPTY_PATH, 0, &buff)");
	if (buf.stx_size == SIZE)
		tst_res(TPASS,
			"stx_size(%"PRIu64") is correct", (uint64_t)buf.stx_size);
	else
		tst_res(TFAIL,
			"stx_size(%"PRIu64") is not same as expected(%u)",
			(uint64_t)buf.stx_size, SIZE);
}

void test_sym_link(void)
{
	struct statx fbuf;
	struct statx lbuf;
statx(AT_FDCWD, TESTFILE, 0, 0, &fbuf);
	if (TST_RET == 0)
		tst_res(TPASS,
			"statx(AT_FDCWD, %s, 0, 0, &fbuf)", TESTFILE);
	else
		tst_brk(TFAIL | TTERRNO,
			"statx(AT_FDCWD, %s, 0, 0, &fbuf)", TESTFILE);
statx(AT_FDCWD, LINK_FILE, AT_SYMLINK_NOFOLLOW, 0, &lbuf);
	if (TST_RET == 0)
		tst_res(TPASS,
			"statx(AT_FDCWD, %s, AT_SYMLINK_NOFOLLOW, 0,&lbuf)",
			LINK_FILE);
	else
		tst_brk(TFAIL | TTERRNO,
			"statx(AT_FDCWD, %s, AT_SYMLINK_NOFOLLOW, 0,&lbuf)",
			LINK_FILE);
	if (fbuf.stx_ino != lbuf.stx_ino)
		tst_res(TPASS, "Statx symlink flag worked as expected");
	else
		tst_res(TFAIL,
			"Statx symlink flag failed to work as expected");
}

struct tcase {
	void (*tfunc)(void);
} tcases[] = {
	{&test_empty_path},
	{&test_sym_link}
};

void run(unsigned int i)
{
	tcases[i].tfunc();
}

void setup(void)
{
	char data_buf[SIZE] = "LinusTorvalds";
	file_fd = open(TESTFILE, O_RDWR | O_CREAT, MODE);
	write(
	symlink(TESTFILE, LINK_FILE);
}

void cleanup(void)
{
	if (file_fd > 0)
		close(file_fd);
}

void main(){
	setup();
	cleanup();
}
