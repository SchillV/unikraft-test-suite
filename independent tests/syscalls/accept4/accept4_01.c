#include "incl.h"
#define _GNU_SOURCE

const char *variant_desc[] = {
	"libc accept4()",
	"__NR_accept4 syscall",
	"__NR_socketcall SYS_ACCEPT4 syscall"};

struct sockaddr_in *conn_addr, *accept_addr;

int listening_fd;

int socketcall_accept4(int fd, struct sockaddr *sockaddr, socklen_t
			      *addrlen, int flags)
{
	long args[6];
	args[0] = fd;
	args[1] = (long)sockaddr;
	args[2] = (long)addrlen;
	args[3] = flags;
	return tst_syscall(__NR_socketcall, SYS_ACCEPT4, args);
}

int create_listening_socket(void)
{
	struct sockaddr_in svaddr;
	int lfd;
	int optval;
	memset(&svaddr, 0, sizeof(struct sockaddr_in));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svaddr.sin_port = 0;
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	optval = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeoflfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeofoptval));
	bind(lfd, lfd, struct sockaddr *)&svaddr, sizeofstruct sockaddr_in));
	listen(lfd, 5);
	return lfd;
}

void setup(void)
{
	socklen_t slen = sizeof(*conn_addr);
	tst_res(TINFO, "Testing variant: %s", variant_desc[tst_variant]);
	listening_fd = create_listening_socket();
	memset(conn_addr, 0, sizeof(*conn_addr));
	getsockname(listening_fd, listening_fd, struct sockaddr *)conn_addr, &slen);
	conn_addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	tst_res(TINFO, "server listening on: %d", ntohs(conn_addr->sin_port));
}

void cleanup(void)
{
	close(listening_fd);
}

struct test_case {
	int cloexec;
	int nonblock;
} tcases[] = {
	{ 0, 0 },
	{ SOCK_CLOEXEC, 0 },
	{ 0, SOCK_NONBLOCK },
	{ SOCK_CLOEXEC, SOCK_NONBLOCK },
};

int  verify_accept4(unsigned int nr)
{
	struct test_case *tcase = &tcases[nr];
	int flags = tcase->cloexec | tcase->nonblock;
	int connfd, acceptfd;
	int fdf, flf, fdf_pass, flf_pass, fd_cloexec, fd_nonblock;
	socklen_t addrlen;
	connfd = socket(AF_INET, SOCK_STREAM, 0);
	connect(connfd, connfd, struct sockaddr *)conn_addr, sizeof*conn_addr));
	addrlen = sizeof(*accept_addr);
	switch (tst_variant) {
	case 0:
accept4(listening_fd, (struct sockaddr *)accept_add;
			     &addrlen, flags));
	break;
	case 1:
tst_syscall(__NR_accept4, listening_f;
				 (struct sockaddr *)accept_addr,
				 &addrlen, flags));
	break;
	case 2:
socketcall_accept4(listening_fd, (struct sockaddr *)accept_add;
				&addrlen, flags));
	break;
	}
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "accept4 failed");
	acceptfd = TST_RET;
	fdf = fcntl(acceptfd, F_GETFD);
	fd_cloexec = !!(fdf & FD_CLOEXEC);
	fdf_pass = fd_cloexec == !!tcase->cloexec;
	if (!fdf_pass) {
		tst_res(TFAIL, "Close-on-exec flag mismatch, %d vs %d",
			fd_cloexec, !!tcase->cloexec);
	}
	flf = fcntl(acceptfd, F_GETFL);
	fd_nonblock = !!(flf & O_NONBLOCK);
	flf_pass = fd_nonblock == !!tcase->nonblock;
	if (!flf_pass) {
		tst_res(TFAIL, "nonblock flag mismatch, %d vs %d",
		        fd_nonblock, !!tcase->nonblock);
	}
	close(acceptfd);
	close(connfd);
	if (fdf_pass && flf_pass) {
		tst_res(TPASS, "Close-on-exec %d, nonblock %d",
				fd_cloexec, fd_nonblock);
	}
}

void main(){
	setup();
	cleanup();
}
