#include <sys/types.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/aio_abi.h>

#define CLEANUP cleanup
int TST_TOTAL = 15;
#ifdef HAVE_LIBAIO

void setup(void);

int myeventfd(unsigned int initval, int flags)
{
	return tst_syscall(__NR_eventfd, initval);
}

int clear_counter(int fd)
{
	uint64_t dummy;
	int ret;
	ret = read(fd, &dummy, sizeof(dummy));
	if (ret == -1) {
		if (errno != EAGAIN) {
			tst_resm(TINFO | TERRNO, "error clearing counter");
			return -1;
		}
	}
	return 0;
}

int set_counter(int fd, uint64_t val)
{
	int ret;
	ret = clear_counter(fd);
	if (ret == -1)
		return -1;
	ret = write(fd, &val, sizeof(val));
	if (ret == -1) {
		tst_resm(TINFO | TERRNO, "error setting counter value");
		return -1;
	}
	return 0;
}

void read_test(int fd, uint64_t required)
{
	int ret;
	uint64_t val;
	ret = read(fd, &val, sizeof(val));
	if (ret == -1) {
		tst_resm(TBROK | TERRNO, "error reading eventfd");
		return;
	}
	if (val == required)
		tst_resm(TPASS, "counter value matches required");
	else
		tst_resm(TFAIL, "counter value mismatch: "
			 "required: %" PRIu64 ", got: %" PRIu64, required, val);
}

void read_eagain_test(int fd)
{
	int ret;
	uint64_t val;
	ret = clear_counter(fd);
	if (ret == -1) {
		tst_resm(TBROK, "error clearing counter");
		return;
	}
	ret = read(fd, &val, sizeof(val));
	if (ret == -1) {
		if (errno == EAGAIN)
			tst_resm(TPASS, "read failed with EAGAIN as expected");
		else
			tst_resm(TFAIL | TERRNO, "read failed (wanted EAGAIN)");
	} else
		tst_resm(TFAIL, "read returned with %d", ret);
}

void write_test(int fd)
{
	int ret;
	uint64_t val;
	val = 12;
	ret = set_counter(fd, val);
	if (ret == -1) {
		tst_resm(TBROK, "error setting counter value to %" PRIu64, val);
		return;
	}
	read_test(fd, val);
}

void write_eagain_test(int fd)
{
	int ret;
	uint64_t val;
	ret = set_counter(fd, UINT64_MAX - 1);
	if (ret == -1) {
		tst_resm(TBROK, "error setting counter value to UINT64_MAX-1");
		return;
	}
	val = 1;
	ret = write(fd, &val, sizeof(val));
	if (ret == -1) {
		if (errno == EAGAIN)
			tst_resm(TPASS, "write failed with EAGAIN as expected");
		else
			tst_resm(TFAIL, "write failed (wanted EAGAIN)");
	} else
		tst_resm(TFAIL, "write returned with %d", ret);
}

void read_einval_test(int fd)
{
	uint32_t invalid;
	int ret;
	ret = read(fd, &invalid, sizeof(invalid));
	if (ret == -1) {
		if (errno == EINVAL)
			tst_resm(TPASS, "read failed with EINVAL as expected");
		else
			tst_resm(TFAIL | TERRNO, "read failed (wanted EINVAL)");
	} else
		tst_resm(TFAIL, "read returned with %d", ret);
}

void write_einval_test(int fd)
{
	uint32_t invalid;
	int ret;
	ret = write(fd, &invalid, sizeof(invalid));
	if (ret == -1) {
		if (errno == EINVAL)
			tst_resm(TPASS, "write failed with EINVAL as expected");
		else
			tst_resm(TFAIL | TERRNO,
				 "write failed (wanted EINVAL)");
	} else
		tst_resm(TFAIL, "write returned with %d", ret);
}

void write_einval2_test(int fd)
{
	int ret;
	uint64_t val;
	ret = clear_counter(fd);
	if (ret == -1) {
		tst_resm(TBROK, "error clearing counter");
		return;
	}
	val = 0xffffffffffffffffLL;
	ret = write(fd, &val, sizeof(val));
	if (ret == -1) {
		if (errno == EINVAL)
			tst_resm(TPASS, "write failed with EINVAL as expected");
		else
			tst_resm(TFAIL | TERRNO,
				 "write failed (wanted EINVAL)");
	} else {
		tst_resm(TFAIL, "write returned with %d", ret);
	}
}

void readfd_set_test(int fd)
{
	int ret;
	fd_set readfds;
	struct timeval timeout = { 0, 0 };
	uint64_t non_zero = 10;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	ret = set_counter(fd, non_zero);
	if (ret == -1) {
		tst_resm(TBROK, "error setting counter value to %" PRIu64,
			 non_zero);
		return;
	}
	ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
	if (ret == -1) {
		tst_resm(TBROK | TERRNO, "select() failed");
		return;
	}
	if (FD_ISSET(fd, &readfds))
		tst_resm(TPASS, "fd is set in readfds");
	else
		tst_resm(TFAIL, "fd is not set in readfds");
}

void readfd_not_set_test(int fd)
{
	int ret;
	fd_set readfds;
	struct timeval timeout = { 0, 0 };
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
	ret = clear_counter(fd);
	if (ret == -1) {
		tst_resm(TBROK, "error clearing counter");
		return;
	}
	ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
	if (ret == -1) {
		tst_resm(TBROK | TERRNO, "select() failed");
		return;
	}
	if (!FD_ISSET(fd, &readfds))
		tst_resm(TPASS, "fd is not set in readfds");
	else
		tst_resm(TFAIL, "fd is set in readfds");
}

void writefd_set_test(int fd)
{
	int ret;
	fd_set writefds;
	struct timeval timeout = { 0, 0 };
	uint64_t non_max = 10;
	FD_ZERO(&writefds);
	FD_SET(fd, &writefds);
	ret = set_counter(fd, non_max);
	if (ret == -1) {
		tst_resm(TBROK, "error setting counter value to %" PRIu64,
			 non_max);
		return;
	}
	ret = select(fd + 1, NULL, &writefds, NULL, &timeout);
	if (ret == -1) {
		tst_resm(TBROK | TERRNO, "select: error getting fd status");
		return;
	}
	if (FD_ISSET(fd, &writefds))
		tst_resm(TPASS, "fd is set in writefds");
	else
		tst_resm(TFAIL, "fd is not set in writefds");
}

void writefd_not_set_test(int fd)
{
	int ret;
	fd_set writefds;
	struct timeval timeout = { 0, 0 };
	FD_ZERO(&writefds);
	FD_SET(fd, &writefds);
	ret = set_counter(fd, UINT64_MAX - 1);
	if (ret == -1) {
		tst_resm(TBROK, "error setting counter value to UINT64_MAX-1");
		return;
	}
	ret = select(fd + 1, NULL, &writefds, NULL, &timeout);
	if (ret == -1) {
		tst_resm(TBROK | TERRNO, "select: error getting fd status");
		return;
	}
	if (!FD_ISSET(fd, &writefds))
		tst_resm(TPASS, "fd is not set in writefds");
	else
		tst_resm(TFAIL, "fd is set in writefds");
}

void child_inherit_test(int fd)
{
	uint64_t val;
	pid_t cpid;
	int ret;
	int status;
	uint64_t to_parent = 0xdeadbeef;
	uint64_t dummy;
	cpid = fork();
	if (cpid == -1)
		tst_resm(TBROK | TERRNO, "fork failed");
	else if (cpid != 0) {
		ret = wait(&status);
		if (ret == -1) {
			tst_resm(TBROK, "error getting child exit status");
			return;
		}
		if (WEXITSTATUS(status) == 1) {
			tst_resm(TBROK, "counter value write not "
				 "successful in child");
			return;
		}
		ret = read(fd, &val, sizeof(val));
		if (ret == -1) {
			tst_resm(TBROK | TERRNO, "error reading eventfd");
			return;
		}
		if (val == to_parent)
			tst_resm(TPASS, "counter value write from "
				 "child successful");
		else
			tst_resm(TFAIL, "counter value write in child "
				 "failed");
	} else {
		ret = read(fd, &dummy, sizeof(dummy));
		if (ret == -1 && errno != EAGAIN) {
			tst_resm(TWARN | TERRNO, "error clearing counter");
			exit(1);
		}
		ret = write(fd, &to_parent, sizeof(to_parent));
		if (ret == -1) {
			tst_resm(TWARN | TERRNO, "error writing eventfd");
			exit(1);
		}
		exit(0);
	}
}
#ifdef HAVE_IO_SET_EVENTFD
        eventfd.

int trigger_eventfd_overflow(int evfd, int *fd, io_context_t * ctx)
{
	int ret;
	struct iocb iocb;
	struct iocb *iocbap[1];
	struct io_event ioev;
	

char buf[4 * 1024];
	ret = io_setup(16, ctx);
	if (ret < 0) {
		errno = -ret;
		if (errno == ENOSYS) {
			tst_brkm(TCONF | TERRNO, cleanup,
				 "io_setup(): AIO not supported by kernel");
		}
		tst_resm(TINFO | TERRNO, "io_setup error");
		return -1;
	}
	if (*fd == -1) {
		tst_resm(TINFO | TERRNO, "open(testfile) failed");
		goto err_io_destroy;
	}
	ret = set_counter(evfd, UINT64_MAX - 1);
	if (ret == -1) {
		tst_resm(TINFO, "error setting counter to UINT64_MAX-1");
		goto err_close_file;
	}
	io_prep_pwrite(&iocb, *fd, buf, sizeof(buf), 0);
	io_set_eventfd(&iocb, evfd);
	iocbap[0] = &iocb;
	ret = io_submit(*ctx, 1, iocbap);
	if (ret < 0) {
		errno = -ret;
		tst_resm(TINFO | TERRNO, "error submitting iocb");
		goto err_close_file;
	}
	ret = io_getevents(*ctx, 1, 1, &ioev, NULL);
	if (ret < 0) {
		errno = -ret;
		tst_resm(TINFO | TERRNO, "error waiting for event");
		goto err_close_file;
	}
	return 0;
err_close_file:
	close(*fd);
err_io_destroy:
	io_destroy(*ctx);
	return -1;
}

void cleanup_overflow(int fd, io_context_t ctx)
{
	close(fd);
	io_destroy(ctx);
}

void overflow_select_test(int evfd)
{
	struct timeval timeout = { 10, 0 };
	fd_set readfds;
	int fd;
	io_context_t ctx;
	int ret;
	ret = trigger_eventfd_overflow(evfd, &fd, &ctx);
	if (ret == -1) {
		tst_resm(TBROK, "error triggering eventfd overflow");
		return;
	}
	FD_ZERO(&readfds);
	FD_SET(evfd, &readfds);
	ret = select(evfd + 1, &readfds, NULL, NULL, &timeout);
	if (ret == -1)
		tst_resm(TBROK | TERRNO,
			 "error getting evfd status with select");
	else {
		if (FD_ISSET(evfd, &readfds))
			tst_resm(TPASS, "read fd set as expected");
		else
			tst_resm(TFAIL, "read fd not set");
	}
	cleanup_overflow(fd, ctx);
}

void overflow_poll_test(int evfd)
{
	struct pollfd pollfd;
	int fd;
	io_context_t ctx;
	int ret;
	ret = trigger_eventfd_overflow(evfd, &fd, &ctx);
	if (ret == -1) {
		tst_resm(TBROK, "error triggering eventfd overflow");
		return;
	}
	pollfd.fd = evfd;
	pollfd.events = POLLIN;
	pollfd.revents = 0;
	ret = poll(&pollfd, 1, 10000);
	if (ret == -1)
		tst_resm(TBROK | TERRNO, "error getting evfd status with poll");
	else {
		if (pollfd.revents & POLLERR)
			tst_resm(TPASS, "POLLERR occurred as expected");
		else
			tst_resm(TFAIL, "POLLERR did not occur");
	}
	cleanup_overflow(fd, ctx);
}

void overflow_read_test(int evfd)
{
	uint64_t count;
	io_context_t ctx;
	int fd;
	int ret;
	ret = trigger_eventfd_overflow(evfd, &fd, &ctx);
	if (ret == -1) {
		tst_resm(TBROK, "error triggering eventfd overflow");
		return;
	}
	ret = read(evfd, &count, sizeof(count));
	if (ret == -1)
		tst_resm(TBROK | TERRNO, "error reading eventfd");
	else {
		if (count == UINT64_MAX)
			tst_resm(TPASS, "overflow occurred as expected");
		else
			tst_resm(TFAIL, "overflow did not occur");
	}
	cleanup_overflow(fd, ctx);
}
#else

void overflow_select_test(int evfd)
{
	tst_resm(TCONF, "eventfd support is not available in AIO subsystem");
}

void overflow_poll_test(int evfd)
{
	tst_resm(TCONF, "eventfd support is not available in AIO subsystem");
}

void overflow_read_test(int evfd)
{
	tst_resm(TCONF, "eventfd support is not available in AIO subsystem");
}
#endif
int main(int argc, char **argv)
{
	int lc;
	int fd;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		int ret;
		uint64_t einit = 10;
		tst_count = 0;
		fd = myeventfd(einit, 0);
		if (fd == -1)
			tst_brkm(TBROK | TERRNO, CLEANUP,
				 "error creating eventfd");
		ret = fcntl(fd, F_SETFL, O_NONBLOCK);
		if (ret == -1)
			tst_brkm(TBROK | TERRNO, CLEANUP,
				 "error setting non-block mode");
		read_test(fd, einit);
		read_eagain_test(fd);
		write_test(fd);
		write_eagain_test(fd);
		read_einval_test(fd);
		write_einval_test(fd);
		write_einval2_test(fd);
		readfd_set_test(fd);
		readfd_not_set_test(fd);
		writefd_set_test(fd);
		writefd_not_set_test(fd);
		child_inherit_test(fd);
		overflow_select_test(fd);
		overflow_poll_test(fd);
		overflow_read_test(fd);
		close(fd);
	}
}

void setup(void)
{
}

void cleanup(void)
{
}
#else
int main(void)
{
	printf("test requires libaio and its development packages\n");
}
#endif

