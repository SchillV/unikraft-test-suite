#include "incl.h"
#define _GNU_SOURCE
#define INVALID_FLAG -1

int old_fd = 3;

int new_fd = 5;

struct tcase {
	int *oldfd;
	int *newfd;
	int flags;
} tcases[] = {
	{&old_fd, &old_fd, O_CLOEXEC},
	{&old_fd, &old_fd, 0},
	{&old_fd, &new_fd, INVALID_FLAG}
};

void run(unsigned int i)
{
	struct tcase *tc = tcases + i;
	TST_EXP_FAIL2(dup3(*tc->oldfd, *tc->newfd, tc->flags), EINVAL,
		"dup3(%d, %d, %d)", *tc->oldfd, *tc->newfd, tc->flags);
}

void main(){
	setup();
	cleanup();
}
