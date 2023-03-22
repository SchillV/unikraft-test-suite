#include "incl.h"
#if defined __i386__ || defined(__x86_64__)

int  verify_iopl(
{
	int total_level = 4;
	int level;
	for (level = 0; level < total_level; ++level) {
iopl(level);
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO, "iopl() failed for level %d, "
					"errno=%d : %s", level,
					TST_ERR, tst_strerrno(TST_ERR));
		} else {
			tst_res(TPASS, "iopl() passed for level %d, "
					"returned %ld", level, TST_RET);
		}
	}
}

void cleanup(void)
{
	 * back to I/O privilege for normal process.
	 */
	if (iopl(0) == -1)
		tst_res(TWARN, "iopl() cleanup failed");
}

void main(){
	setup();
	cleanup();
}
