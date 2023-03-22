#include "incl.h"

time_t new_time;

void run(void)
{
do_stime(&new_time);
	if (TST_RET != -1) {
		tst_res(TFAIL,
			"stime() returned %ld, expected -1 EPERM", TST_RET);
		return;
	}
	if (TST_ERR == EPERM) {
		tst_res(TPASS | TTERRNO, "stime(2) fails, Caller not root");
		return;
	}
	tst_res(TFAIL | TTERRNO,
		"stime(2) fails, Caller not root, expected errno:%d", EPERM);
}

void setup(void)
{
	time_t curr_time;
	struct passwd *ltpuser;
	stime_info();
	ltpuser = getpwnam("nobody");
	setuid(ltpuser->pw_uid);
	if ((curr_time = time(NULL)) < 0)
		tst_brk(TBROK | TERRNO, "time() failed");
	new_time = curr_time + 10;
}

void main(){
	setup();
	cleanup();
}
