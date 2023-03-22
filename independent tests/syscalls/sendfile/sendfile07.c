#include "incl.h"
#define MAX_FILL_DATA_LENGTH 0xFFFFFFF

int p[2];

int in_fd;

int out_fd;

void setup(void)
{
	int i;
	tst_fill_file("in_file", 'a', 10, 1);
	in_fd = open("in_file", O_RDONLY);
	socketpair(PF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0, p);
	out_fd = p[1];
	for (i = 0; i < MAX_FILL_DATA_LENGTH; ++i) {
write(out_fd, "a", 1);
		if (TST_RET < 0) {
			if (TST_ERR == EAGAIN)
				return;
			else
				tst_brk(TBROK | TTERRNO, "write(out_fd, buf, 1)");
		}
	}
	tst_brk(TBROK, "Failed to get EAGAIN after %i bytes",
	        MAX_FILL_DATA_LENGTH);
}

void cleanup(void)
{
	if (p[0])
		close(p[0]);
	if (p[1])
		close(p[1]);
	close(in_fd);
}

void run(void)
{
	TST_EXP_FAIL(sendfile(out_fd, in_fd, NULL, 1), EAGAIN,
		     "sendfile(out_fd, in_fd, NULL, 1) with blocked out_fd");
}

void main(){
	setup();
	cleanup();
}
