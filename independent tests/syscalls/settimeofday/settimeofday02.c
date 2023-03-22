#include "incl.h"

struct tcase {
	struct timeval tv;
	int exp_errno;
	char *message;
} tcases[] = {
	{{-1, 0}, EINVAL, "tv.tv_sec is negative"},
	{{0, -1}, EINVAL, "tv.tv_usec is outside the range [0..999,999]"},
	{{100, 100}, EPERM, "calling process without CAP_SYS_TIME capability"},
};

int  verify_settimeofday(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	tst_res(TINFO, "%s", tc->message);
settimeofday(&tc->tv, NULL);
	if (TST_RET != -1) {
		tst_res(TFAIL, "settimeofday() succeeded unexpectedly");
		return;
	}
	if (TST_ERR != tc->exp_errno)
		tst_res(TFAIL | TTERRNO, "Expected %s got ", tst_strerrno(tc->exp_errno));
	else
		tst_res(TPASS | TTERRNO, "Received expected errno");
}

void main(){
	setup();
	cleanup();
}
