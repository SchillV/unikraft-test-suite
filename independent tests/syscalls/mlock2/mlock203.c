#include "incl.h"

unsigned long pgsz;

char *addr;

int  verify_mlock2(
{
	unsigned long bsize, asize1, asize2;
	file_lines_scanf("/proc/self/status", "VmLck: %lu", &bsize);
tst_syscall(__NR_mlock2, addr, pgsz, MLOCK_ONFAULT);
	if (TST_RET != 0) {
		if (TST_ERR == EINVAL) {
			tst_res(TCONF,
				"mlock2() didn't support MLOCK_ONFAULT");
		} else {
			tst_res(TFAIL | TTERRNO,
				"mlock2(MLOCK_ONFAULT) failed");
		}
		return;
	}
	file_lines_scanf("/proc/self/status", "VmLck: %lu", &asize1);
	if ((asize1 - bsize) * 1024 != pgsz) {
		tst_res(TFAIL,
			"mlock2(MLOCK_ONFAULT) locked %lu size, expected %lu",
			(asize1 - bsize) * 1024, pgsz);
		goto end;
	}
tst_syscall(__NR_mlock2, addr, pgsz, 0);
	if (TST_RET != 0) {
		tst_res(TFAIL | TTERRNO, "mlock2() failed");
		goto end;
	}
	file_lines_scanf("/proc/self/status", "VmLck: %lu", &asize2);
	if (asize1 != asize2) {
		tst_res(TFAIL, "Locking one memory again increased VmLck");
	} else {
		tst_res(TPASS,
			"Locking one memory again didn't increased VmLck");
	}
end:
	munlock(addr, pgsz);
}

void setup(void)
{
	pgsz = getpagesize();
	addr = mmap(NULL, pgsz, PROT_WRITE,
			 MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
}

void cleanup(void)
{
	if (addr)
		munmap(addr, pgsz);
}

void main(){
	setup();
	cleanup();
}
