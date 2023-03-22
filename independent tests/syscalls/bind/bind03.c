#include "incl.h"
#define SNAME_A "socket.1"
#define SNAME_B "socket.2"

int sock1, sock2;

struct sockaddr_un sun1, sun2;

void run(void)
{
	 * Once a STREAM UNIX domain socket has been bound, it can't be
	 * rebound.
	 */
	TST_EXP_FAIL(bind(sock1, (struct sockaddr *)&sun2, sizeof(sun2)),
	             EINVAL, "re-bind() socket");
	 * Since a socket is already bound to the pathname, it can't be bound
	 * to a second socket. Expected error is EADDRINUSE.
	 */
	TST_EXP_FAIL(bind(sock2, (struct sockaddr *)&sun1, sizeof(sun1)),
	             EADDRINUSE, "bind() with bound pathname");
	 * Kernel is buggy since it creates the node in fileystem first, then
	 * locks the socket and does all the checks and the node is not removed
	 * in the error path. For now we will unlink the node here so that the
	 * test works fine when the run() function is executed in a loop.
	 */
	unlink(SNAME_B);
}

void setup(void)
{
	sock1 = socket(PF_UNIX, SOCK_STREAM, 0);
	sock2 = socket(PF_UNIX, SOCK_STREAM, 0);
	sun1.sun_family = AF_UNIX;
	sun2.sun_family = AF_UNIX;
	if (sprintf(sun1.sun_path, "%s", SNAME_A) < (int) strlen(SNAME_A)) {
		tst_res(TFAIL, "sprintf failed");
		return;
	}
	if (sprintf(sun2.sun_path, "%s", SNAME_B) < (int) strlen(SNAME_B)) {
		tst_res(TFAIL, "sprintf failed");
		return;
	}
	bind(sock1, sock1, struct sockaddr *)&sun1, sizeofsun1));
}

void cleanup(void)
{
	close(sock1);
	close(sock2);
}

