#include "incl.h"

int fd[2] = {-1, -1};

int nfd[2] = {10, 20};

void setup(void)
{
	pipe(fd);
}

void cleanup(void)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(fd); i++) {
		close(fd[i]);
		close(nfd[i]);
	}
}

void run(unsigned int i)
{
	struct stat oldbuf, newbuf;
	TST_EXP_VAL(dup2(fd[i], nfd[i]), nfd[i]);
	if (TST_RET == -1)
		return;
	fstat(fd[i], &oldbuf);
	fstat(nfd[i], &newbuf);
	TST_EXP_EQ_LU(oldbuf.st_ino, newbuf.st_ino);
	close(TST_RET);
}

void main(){
	setup();
	cleanup();
}
