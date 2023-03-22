#include "incl.h"

int  verify_swapoff(
{
	if (tst_syscall(__NR_swapon, "./swapfile01", 0) != 0) {
		tst_res(TFAIL | TERRNO, "Failed to turn on the swap file"
			 ", skipping test iteration");
		return;
	}
tst_syscall(__NR_swapoff, "./swapfile01");
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "Failed to turn off swapfile,"
			" system reboot after execution of LTP "
			"test suite is recommended.");
	} else {
		tst_res(TPASS, "Succeeded to turn off swapfile");
	}
}

void setup(void)
{
	is_swap_supported("./tstswap");
	if (!tst_fs_has_free(".", 64, TST_MB))
		tst_brk(TBROK,
			"Insufficient disk space to create swap file");
	if (tst_fill_file("swapfile01", 0x00, 1024, 65536))
		tst_brk(TBROK, "Failed to create file for swap");
	if (system("mkswap swapfile01 > tmpfile 2>&1") != 0)
		tst_brk(TBROK, "Failed to make swapfile");
}

void main(){
	setup();
	cleanup();
}
