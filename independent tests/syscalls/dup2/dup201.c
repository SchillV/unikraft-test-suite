#include "incl.h"

int maxfd, mystdout;

int goodfd = 5;

int badfd = -1;

struct tcase {
	int *ofd;
	int *nfd;
} tcases[] = {
	{&badfd, &goodfd},
	{&maxfd, &goodfd},
	{&mystdout, &badfd},
	{&mystdout, &maxfd},
};

void setup(void)
{
	maxfd = getdtablesize();
}

void run(unsigned int i)
{
	struct tcase *tc = tcases + i;
	TST_EXP_FAIL2(dup2(*tc->ofd, *tc->nfd), EBADF,
			"dup2(%d, %d)", *tc->ofd, *tc->nfd);
}

void main(){
	setup();
	cleanup();
}
