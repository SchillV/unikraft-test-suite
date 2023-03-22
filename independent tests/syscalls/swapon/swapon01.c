#include "incl.h"

int  verify_swapon(
{
tst_syscall(__NR_swapon, "./swapfile01", 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "Failed to turn on swapfile");
	} else {
		tst_res(TPASS, "Succeeded to turn on swapfile");
		if (tst_syscall(__NR_swapoff, "./swapfile01") != 0) {
			tst_brk(TBROK | TERRNO, "Failed to turn off swapfile,"
				" system reboot after execution of LTP "
				"test suite is recommended.");
		}
	}
}

void setup(void)
{
	is_swap_supported("./tstswap");
	make_swapfile("swapfile01", 0);
}

void main(){
	setup();
	cleanup();
}
