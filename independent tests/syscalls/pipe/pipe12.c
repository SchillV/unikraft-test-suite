#include "incl.h"
#define _GNU_SOURCE

int fds[2];

char *wrbuf;

char *rdbuf;

ssize_t max_size, invalid_size;

struct tcase {
	int full_flag;
	int offset;
	char *message;
	int check_flag;
} tcases[] = {
	{1, 0, "Write to full pipe", 1},
	 * For a non-empty(unaligned page size) pipe, the sequent large size
	 * write(>page_size)will use new pages. So it may exist a hole in
	 * page and we print this value instead of checking it.
	 */
	{0, 1, "Write to non-empty pipe", 0},
	{0, 0, "Write to empty pipe", 1},
};

int  verify_pipe(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int nbytes;
	memset(rdbuf, 0, max_size);
	tst_res(TINFO, "%s", tc->message);
	if (tc->full_flag) {
		write(1, fds[1], wrbuf, max_size);
write(fds[1], "x", 1);
		if (TST_RET != -1) {
			tst_res(TFAIL, "write succeeded unexpectedly");
			goto clean_pipe_buf;
		}
		if (TST_ERR == EAGAIN)
			tst_res(TPASS | TTERRNO, "write failed as expected");
		else
			tst_res(TFAIL | TTERRNO, "write failed, expected EAGAIN but got");
	} else {
		write(1, fds[1], "x", tc->offset);
write(fds[1], wrbuf, invalid_size);
		if (TST_RET == -1) {
			tst_res(TFAIL, "write failed unexpectedly");
			goto clean_pipe_buf;
		}
		tst_res(TPASS, "write succeeded as expectedly");
	}
	ioctl(fds[1], FIONREAD, &nbytes);
	if (tc->check_flag) {
		if (nbytes == max_size - tc->offset)
			tst_res(TPASS, "write %d bytes", nbytes);
		else
			tst_res(TFAIL, "write expected %ld bytes, got %d bytes",
					max_size, nbytes);
	} else
		tst_res(TPASS, "write %d bytes", nbytes);
clean_pipe_buf:
	read(0, fds[0], rdbuf, max_size);
}

void cleanup(void)
{
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[1] > 0)
		close(fds[1]);
	if (wrbuf)
		free(wrbuf);
	if (rdbuf)
		free(rdbuf);
}

void setup(void)
{
	pipe(fds);
	max_size = fcntl(fds[1], F_GETPIPE_SZ);
	invalid_size = max_size + 4096;
	wrbuf = malloc(invalid_size);
	rdbuf = malloc(max_size);
	memset(wrbuf, 'x', invalid_size);
	fcntl(fds[1], F_SETFL, O_NONBLOCK);
	fcntl(fds[0], F_SETFL, O_NONBLOCK);
}

void main(){
	setup();
	cleanup();
}
