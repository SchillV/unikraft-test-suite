#include "incl.h"

char fname[20] = "testfile";

int fd = -1, max_files;

int  verify_fcntl()
{
	pid_t pid;
	int i;
	pid = fork();
	if (pid == 0) {
		for (i = 0; i < max_files; i++) {
			fd = open(fname, O_CREAT | O_RDONLY, 0444);
			if (fd == -1)
				break;
		}
		int ret = fcntl(1, F_DUPFD, 1);
		if(ret== -1 && errno == EMFILE)
			return 1;
	}
	return 0;
}

void setup(void)
{
}

void cleanup(void)
{
	if (fd > -1)
		close(fd);
	unlink(fname);
}

void main(){
	setup();
	if(verify_fcntl())
		printf("test passed\n");
	cleanup();
}
