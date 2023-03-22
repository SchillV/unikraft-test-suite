#include "incl.h"
time_t tlocal;
time_t *targs[] = {
	NULL, &tlocal,
};

int  verify_time(unsigned int i)
{
	time_t *tloc = targs[i];
time(tloc);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "time()");
		return;
	}
	if (!tloc)
		tst_res(TPASS, "time() returned value %ld", TST_RET);
	else if (*tloc == TST_RET)
		tst_res(TPASS,
			"time() returned value %ld, stored value %jd are same",
			TST_RET, (intmax_t) *tloc);
	else
		tst_res(TFAIL,
			"time() returned value %ld, stored value %jd are different",
			TST_RET, (intmax_t) *tloc);
}

void main(){
	setup();
	cleanup();
}
