#include "incl.h"
#define _GNU_SOURCE
#define TESTFILE "vmsplice_test_file"

int fd_out;

char buffer[TEST_BLOCK_SIZE];

void check_file(void)
{
	int i;
	char vmsplicebuffer[TEST_BLOCK_SIZE];
	fd_out = open(TESTFILE, O_RDONLY);
	read(1, fd_out, vmsplicebuffer, TEST_BLOCK_SIZE);
	for (i = 0; i < TEST_BLOCK_SIZE; i++) {
		if (buffer[i] != vmsplicebuffer[i])
			break;
	}
	if (i < TEST_BLOCK_SIZE)
		tst_res(TFAIL, "Wrong data read from the buffer at %i", i);
	else
		tst_res(TPASS, "Written data has been read back correctly");
	close(fd_out);
}

void vmsplice_test(void)
{
	int pipes[2];
	long written;
	int ret;
	int fd_out;
	struct iovec v;
	loff_t offset;
	v.iov_base = buffer;
	v.iov_len = TEST_BLOCK_SIZE;
	fd_out = open(TESTFILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	pipe(pipes);
	struct pollfd pfd = {.fd = pipes[1], .events = POLLOUT};
	offset = 0;
	while (v.iov_len) {
		 * in a real app you'd be more clever with poll of course,
		 * here we are basically just blocking on output room and
		 * not using the free time for anything interesting.
		 */
		if (poll(&pfd, 1, -1) < 0)
			tst_brk(TBROK | TERRNO, "poll() failed");
		written = vmsplice(pipes[1], &v, 1, 0);
		if (written < 0) {
			tst_brk(TBROK | TERRNO, "vmsplice() failed");
		} else {
			if (written == 0) {
				break;
			} else {
				v.iov_base += written;
				v.iov_len -= written;
			}
		}
		ret = splice(pipes[0], NULL, fd_out, &offset, written, 0);
		if (ret < 0)
			tst_brk(TBROK | TERRNO, "splice() failed");
	}
	close(pipes[0]);
	close(pipes[1]);
	close(fd_out);
	check_file();
}

void setup(void)
{
	int i;
	for (i = 0; i < TEST_BLOCK_SIZE; i++)
		buffer[i] = i & 0xff;
}

void cleanup(void)
{
	if (fd_out > 0)
		close(fd_out);
}

void main(){
	setup();
	cleanup();
}
