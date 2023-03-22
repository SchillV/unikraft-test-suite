#include "incl.h"
#define _GNU_SOURCE

int socket_fd, accepted;

struct sockaddr_un sun;
#define SOCKNAME	"testsocket"

void setup(void)
{
	sun.sun_family = AF_UNIX;
	(void)strcpy(sun.sun_path, SOCKNAME);
	socket_fd = socket(sun.sun_family, SOCK_STREAM, 0);
	bind(socket_fd, socket_fd, struct sockaddr *)&sun, sizeofsun));
	listen(socket_fd, SOMAXCONN);
}

void fork_func(void)
{
	int fork_socket_fd = socket(sun.sun_family, SOCK_STREAM, 0);
	connect(fork_socket_fd, fork_socket_fd, struct sockaddr *)&sun, sizeofsun));
	TST_CHECKPOINT_WAIT(0);
	close(fork_socket_fd);
	exit(0);
}

void test_function(void)
{
	pid_t fork_id;
	struct ucred cred;
	socklen_t cred_len = sizeof(cred);
	fork_id = fork();
	if (!fork_id)
		fork_func();
	accepted = accept(socket_fd, NULL, NULL);
	if (accepted < 0) {
		tst_res(TFAIL | TERRNO, "Error with accepting connection");
		goto clean;
	}
	if (getsockopt(accepted, SOL_SOCKET,
				SO_PEERCRED, &cred, &cred_len) < 0) {
		tst_res(TFAIL | TERRNO, "Error while getting socket option");
		goto clean;
	}
	if (fork_id != cred.pid)
		tst_res(TFAIL, "Received wrong PID %d, expected %d",
				cred.pid, getpid());
	else
		tst_res(TPASS, "Test passed");
clean:
	if (accepted >= 0)
		close(accepted);
	TST_CHECKPOINT_WAKE(0);
}

void cleanup(void)
{
	if (accepted >= 0)
		close(accepted);
	if (socket_fd >= 0)
		close(socket_fd);
}

void main(){
	setup();
	cleanup();
}
