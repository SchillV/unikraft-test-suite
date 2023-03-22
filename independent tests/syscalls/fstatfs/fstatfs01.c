#include "incl.h"
#define MNT_POINT "mntpoint"
#define TEMP_FILE MNT_POINT"/test_file"

int file_fd;

int pipe_fd;

struct tcase {
	int *fd;
	const char *msg;
} tcases[] = {
	{&file_fd, "fstatfs() on a file"},
	{&pipe_fd, "fstatfs() on a pipe"},
};

void run(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	struct statfs buf;
	TST_EXP_PASS(fstatfs(*tc->fd, &buf), "%s", tc->msg);
}

void setup(void)
{
	int pipe[2];
	file_fd = open(TEMP_FILE, O_RDWR | O_CREAT, 0700);
	pipe(pipe);
	pipe_fd = pipe[0];
	close(pipe[1]);
}

void cleanup(void)
{
	if (file_fd > 0)
		close(file_fd);
	if (pipe_fd > 0)
		close(pipe_fd);
}

void main(){
	setup();
	cleanup();
}
