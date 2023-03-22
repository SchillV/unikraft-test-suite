#include "incl.h"

int in_fd;

int out_fd;

int negative_fd = -1;
struct test_case_t {
	int *in_fd;
	int *out_fd;
	const char *desc;
} tc[] = {
	{&in_fd, &negative_fd, "out_fd=-1"},
	{&in_fd, &in_fd, "out_fd=O_RDONLY"},
	{&negative_fd, &out_fd, "in_fd=-1"},
	{&out_fd, &out_fd, "out_fd=O_WRONLY"},
};

void setup(void)
{
	in_fd = open("in_file", O_CREAT | O_RDONLY, 0600);
	out_fd = creat("out_file", 0600);
}

void cleanup(void)
{
	close(in_fd);
	close(out_fd);
}

void run(unsigned int i)
{
	TST_EXP_FAIL2(sendfile(*(tc[i].out_fd), *(tc[i].in_fd), NULL, 1),
		     EBADF, "sendfile(..) with %s", tc[i].desc);
}

void main(){
	setup();
	cleanup();
}
