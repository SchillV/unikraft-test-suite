#include "incl.h"

char buffer[5];

struct t_case {
	char *buf;
	size_t size;
	int exp_err;
} tcases[] = {
	{(void *)-1, PATH_MAX, EFAULT},
	{NULL, (size_t)-1, ENOMEM},
	{buffer, 0, EINVAL},
	{buffer, 1, ERANGE},
	{NULL, 1, ERANGE}
};

int  verify_getcwd(unsigned int n)
{
	struct t_case *tc = &tcases[n];
	char *res;
	errno = 0;
	res = getcwd(tc->buf, tc->size);
	TST_ERR = errno;
	if (res) {
		tst_res(TFAIL, "getcwd() succeeded unexpectedly");
		return;
	}
	if (TST_ERR != tc->exp_err) {
		tst_res(TFAIL | TTERRNO, "getcwd() failed unexpectedly, expected %s",
			tst_strerrno(tc->exp_err));
		return;
	}
	tst_res(TPASS | TTERRNO, "getcwd() failed as expected");
}

void main(){
	setup();
	cleanup();
}
