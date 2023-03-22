#include "incl.h"
#define _GNU_SOURCE

int fds[2];

int flags;

void test_pipe2(void)
{
	int ret;
	pid_t pid;
	 * This ensures parent process is still in non-block mode when
	 * using -i parameter. Subquent writes hould return -1 and errno
	 * set to either EAGAIN or EWOULDBLOCK because pipe is already full.
	 */
	fcntl(fds[1], F_SETFL, flags | O_NONBLOCK);
	ret = write(fds[1], "x", 1);
	if (ret == -1) {
		if (errno == EAGAIN)
			tst_res(TPASS | TERRNO, "write failed as expected");
		else
			tst_brk(TFAIL | TERRNO, "write failed expected EAGAIN but got");
	} else {
		tst_res(TFAIL, "write() succeeded unexpectedly");
	}
	pid = fork();
	if (!pid) {
		fcntl(fds[1], F_SETFL, flags & ~O_NONBLOCK);
		write(1, fds[1], "x", 1);
	}
	if (TST_PROCESS_STATE_WAIT(pid, 'S', 1000) < 0)
		tst_res(TFAIL, "Child process is not blocked");
	else
		tst_res(TPASS, "Child process is blocked");
	kill(pid, SIGKILL);
	wait(NULL);
}

void setup(void)
{
	int page_size, pipe_size;
	char *write_buffer;
	pipe2(fds, O_NONBLOCK);
	page_size = sysconf(_SC_PAGESIZE);
	flags = fcntl(fds[1], F_GETFL);
	if (!(flags & O_NONBLOCK))
		tst_brk(TCONF, "O_NONBLOCK flag must be set");
	 * A pipe has two file descriptors.
	 * But in the kernel these two file descriptors point to the same pipe.
	 * So setting size from first file handle set size for the pipe.
	 */
	fcntl(fds[0], F_SETPIPE_SZ, 0);
	 * So getting size from the second file descriptor return the size of
	 * the pipe which was changed before with first file descriptor.
	 */
	pipe_size = fcntl(fds[1], F_GETPIPE_SZ);
	if (pipe_size != page_size)
		tst_res(TFAIL, "Pipe size (%d) must be page size (%d)",
				pipe_size, page_size);
	write_buffer = malloc(pipe_size);
	memset(write_buffer, 'x', pipe_size);
	write(1, fds[1], write_buffer, pipe_size);
	free(write_buffer);
}

void cleanup(void)
{
	if (fds[0] > 0)
		close(fds[0]);
	if (fds[1] > 0)
		close(fds[1]);
}

void main(){
	setup();
	cleanup();
}
