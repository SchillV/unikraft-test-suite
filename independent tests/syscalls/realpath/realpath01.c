#include "incl.h"
#define CHROOT_DIR "cve-2018-1000001"

void setup(void)
{
	mkdir(CHROOT_DIR, 0755);
	chroot(CHROOT_DIR);
}

void run(void)
{
	TESTPTR(realpath(".", NULL));
	if (TST_ERR != ENOENT) {
		tst_res(TFAIL | TTERRNO, "returned unexpected errno");
	} else	if (TST_RET_PTR != NULL) {
		tst_res(TFAIL, "syscall didn't return NULL: '%s'",
				(char *)TST_RET_PTR);
	} else {
		tst_res(TPASS, "bug not reproduced");
	}
}

void main(){
	setup();
	cleanup();
}
