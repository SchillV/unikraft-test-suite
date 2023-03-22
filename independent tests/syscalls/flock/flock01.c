#include "incl.h"

int fd = -1;

struct tcase {
	int operation;
	char *opt;
} tcases[] = {
	{LOCK_SH, "Shared Lock" },
	{LOCK_UN, "Unlock"},
	{LOCK_EX, "Exclusive Lock"},
};

int  verify_flock(unsigned n)
{
	struct tcase *tc = &tcases[n];
flock(fd, tc->operation);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO,
			"flock() failed to get %s", tc->opt);
	} else {
		tst_res(TPASS,
			"flock() succeeded with %s", tc->opt);
	}
}

void setup(void)
{
	fd = open("testfile", O_CREAT | O_TRUNC | O_RDWR, 0644);
}

void cleanup(void)
{
	if (fd >= 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
