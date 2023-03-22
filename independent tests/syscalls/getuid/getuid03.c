#include "incl.h"

int  verify_getuid(
{
	long uid;
	TST_EXP_POSITIVE(GETUID(), "getuid()");
	if (!TST_PASS)
		return;
	file_lines_scanf("/proc/self/status", "Uid: %ld", &uid);
	if (TST_RET != uid) {
		tst_res(TFAIL,
			"getuid() ret %ld != /proc/self/status Uid: %ld",
			TST_RET, uid);
	} else {
		tst_res(TPASS,
			"getuid() ret == /proc/self/status Uid: %ld", uid);
	}
}

void main(){
	setup();
	cleanup();
}
