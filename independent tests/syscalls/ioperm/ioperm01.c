#include "incl.h"
#if defined __i386__ || defined(__x86_64__)
unsigned long io_addr;
#define NUM_BYTES 3
#ifndef IO_BITMAP_BITS
#define IO_BITMAP_BITS 1024
#endif

int  verify_ioperm(
{
ioperm(io_addr, NUM_BYTES, 1);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "ioperm() failed for port address "
				"%lu,  errno=%d : %s", io_addr,
				TST_ERR, tst_strerrno(TST_ERR));
	} else {
		tst_res(TPASS, "ioperm() passed for port "
				"address %lu, returned %lu",
				io_addr, TST_RET);
	}
}

void setup(void)
{
	io_addr = IO_BITMAP_BITS - NUM_BYTES;
}

void cleanup(void)
{
	 * Reset I/O privileges for the specified port.
	 */
	if ((ioperm(io_addr, NUM_BYTES, 0)) == -1)
		tst_brk(TBROK | TERRNO, "ioperm() cleanup failed");
}

void main(){
	setup();
	cleanup();
}
