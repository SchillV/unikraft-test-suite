#include "incl.h"

struct timeval real_time_tv;

void run(void)
{
	time_t new_time;
	struct timeval pres_time_tv;
	if (gettimeofday(&real_time_tv, NULL) < 0)
		tst_brk(TBROK | TERRNO, "gettimeofday() failed");
	new_time = real_time_tv.tv_sec + 30;
	if (do_stime(&new_time) < 0) {
		tst_res(TFAIL | TERRNO, "stime(%ld) failed", new_time);
		return;
	}
	if (gettimeofday(&pres_time_tv, NULL) < 0)
		tst_brk(TBROK | TERRNO, "gettimeofday() failed");
	switch (pres_time_tv.tv_sec - new_time) {
	case 0:
	case 1:
		tst_res(TINFO, "pt.tv_sec: %ld", pres_time_tv.tv_sec);
		tst_res(TPASS, "system time was set to %ld", new_time);
	break;
	default:
		tst_res(TFAIL, "system time not set to %ld (got: %ld)",
			new_time, pres_time_tv.tv_sec);
	}
}

void setup(void)
{
	stime_info();
}

void main(){
	setup();
	cleanup();
}
