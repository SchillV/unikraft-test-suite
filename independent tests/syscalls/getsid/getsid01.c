#include "incl.h"

pid_t p_sid;

void run(void)
{
	pid_t pid, c_sid;
getsid(0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "getsid(0) failed in parent");
		return;
	}
	p_sid = TST_RET;
	pid = fork();
	if (pid == 0) {
getsid(0);
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO, "getsid(0) failed in child");
			return;
		}
		c_sid = TST_RET;
		TST_EXP_EQ_LI(p_sid, c_sid);
	} else {
		waitpid(pid, NULL, 0);
	}
}

void main(){
	setup();
	cleanup();
}
