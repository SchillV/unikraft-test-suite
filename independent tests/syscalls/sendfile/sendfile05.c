#include "incl.h"

int in_fd;

int out_fd;

void setup(void)
{
	in_fd = open("in_file", O_CREAT | O_RDWR, 0600);
	out_fd = creat("out_file", 0600);
}

void cleanup(void)
{
	close(in_fd);
	close(out_fd);
}

void run(void)
{
	off_t offset = -1;
	TST_EXP_FAIL(sendfile(out_fd, in_fd, &offset, 1), EINVAL,
		     "sendfile(out, in, &offset, ..) with offset=%lld",
		     (long long int)offset);
}

void main(){
	setup();
	cleanup();
}
