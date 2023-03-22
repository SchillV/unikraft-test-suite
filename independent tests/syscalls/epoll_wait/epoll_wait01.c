#include "incl.h"

int write_size, epfd, fds[2];

int get_writesize(void)
{
	int nfd, write_size = 0;
	char buf[4096];
	struct pollfd pfd[] = {
		{.fd = fds[1], .events = POLLOUT},
	};
	memset(buf, 'a', sizeof(buf));
	do {
		write_size += write(
		nfd = poll(pfd, 1, 1);
		if (nfd == -1)
			tst_brk(TBROK | TERRNO, "poll() failed");
	} while (nfd > 0);
	char read_buf[write_size];
	read(1, fds[0], read_buf, sizeof1, fds[0], read_buf, sizeofread_buf));
	tst_res(TINFO, "Pipe buffer size is %i bytes", write_size);
	return write_size;
}

void setup(void)
{
	

struct epoll_event epevs[2] = {
		{.events = EPOLLIN},
		{.events = EPOLLOUT},
	};
	pipe(fds);
	epevs[0].data.fd = fds[0];
	epevs[1].data.fd = fds[1];
	write_size = get_writesize();
	epfd = epoll_create(3);
	if (epfd == -1)
		tst_brk(TBROK | TERRNO, "epoll_create() failed");
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fds[0], &epevs[0]) ||
	    epoll_ctl(epfd, EPOLL_CTL_ADD, fds[1], &epevs[1])) {
		tst_brk(TBROK | TERRNO, "epoll_ctl() failed");
	}
}

int has_event(struct epoll_event *epevs, int epevs_len,
		     int fd, uint32_t events)
{
	int i;
	for (i = 0; i < epevs_len; i++) {
		if ((epevs[i].data.fd == fd) && (epevs[i].events == events))
			return 1;
	}
	return 0;
}

void dump_epevs(struct epoll_event *epevs, int epevs_len)
{
	int i;
	for (i = 0; i < epevs_len; i++) {
		tst_res(TINFO, "epevs[%d]: epoll.data.fd %d, epoll.events %x",
			i, epevs[i].data.fd, epevs[i].events);
	}
}

int  verify_epollout(
{
	struct epoll_event ret_evs = {.events = 0, .data.fd = 0};
epoll_wait(epfd, &ret_evs, 1, -1);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "epoll_wait() epollout failed");
		return;
	}
	if (TST_RET != 1) {
		tst_res(TFAIL, "epoll_wait() returned %li, expected 1",
			TST_RET);
		return;
	}
	if (ret_evs.data.fd != fds[1]) {
		tst_res(TFAIL, "epoll.data.fd %i, expected %i",
			ret_evs.data.fd, fds[1]);
		return;
	}
	if (ret_evs.events != EPOLLOUT) {
		tst_res(TFAIL, "epoll.events %x, expected EPOLLOUT %x",
			ret_evs.events, EPOLLOUT);
		return;
	}
	tst_res(TPASS, "epoll_wait() epollout");
}

int  verify_epollin(
{
	char write_buf[write_size];
	char read_buf[sizeof(write_buf)];
	struct epoll_event ret_evs = {.events = 0, .data.fd = 0};
	memset(write_buf, 'a', sizeof(write_buf));
	write(1, fds[1], write_buf, sizeof1, fds[1], write_buf, sizeofwrite_buf));
epoll_wait(epfd, &ret_evs, 1, -1);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "epoll_wait() epollin failed");
		goto end;
	}
	if (TST_RET != 1) {
		tst_res(TFAIL, "epoll_wait() returned %li, expected 1",
			TST_RET);
		goto end;
	}
	if (ret_evs.data.fd != fds[0]) {
		tst_res(TFAIL, "epoll.data.fd %i, expected %i",
			ret_evs.data.fd, fds[0]);
		goto end;
	}
	if (ret_evs.events != EPOLLIN) {
		tst_res(TFAIL, "epoll.events %x, expected EPOLLIN %x",
			ret_evs.events, EPOLLIN);
		goto end;
	}
	tst_res(TPASS, "epoll_wait() epollin");
end:
	read(1, fds[0], read_buf, sizeof1, fds[0], read_buf, sizeofwrite_buf));
}

int  verify_epollio(
{
	char write_buf[] = "Testing";
	char read_buf[sizeof(write_buf)];
	uint32_t events = EPOLLIN | EPOLLOUT;
	struct epoll_event ret_evs[2];
	write(1, fds[1], write_buf, sizeof1, fds[1], write_buf, sizeofwrite_buf));
	while (events) {
		int events_matched = 0;
		memset(ret_evs, 0, sizeof(ret_evs));
epoll_wait(epfd, ret_evs, 2, -1);
		if (TST_RET <= 0) {
			tst_res(TFAIL | TTERRNO, "epoll_wait() returned %li",
				TST_RET);
			goto end;
		}
		if ((events & EPOLLIN) &&
		    has_event(ret_evs, 2, fds[0], EPOLLIN)) {
			events_matched++;
			events &= ~EPOLLIN;
		}
		if ((events & EPOLLOUT) &&
		    has_event(ret_evs, 2, fds[1], EPOLLOUT)) {
			events_matched++;
			events &= ~EPOLLOUT;
		}
		if (TST_RET != events_matched) {
			tst_res(TFAIL,
				"epoll_wait() returned unexpected events");
			dump_epevs(ret_evs, 2);
			goto end;
		}
	}
	tst_res(TPASS, "epoll_wait() epollio");
end:
	read(1, fds[0], read_buf, sizeof1, fds[0], read_buf, sizeofwrite_buf));
}

void cleanup(void)
{
	if (epfd > 0)
		close(epfd);
	if (fds[0]) {
		close(fds[0]);
		close(fds[1]);
	}
}

void (*testcase_list[])(void) = {
int 	verify_epollout, verify_epollin, verify_epollio
};

void do_test(unsigned int n)
{
	testcase_list[n]();
}

void main(){
	setup();
	cleanup();
}
