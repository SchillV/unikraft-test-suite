#include "incl.h"
#if defined __i386__ || defined(__x86_64__)
#define NUM_BYTES 3
#ifndef IO_BITMAP_BITS
#define IO_BITMAP_BITS_16 65536
#endif

struct tcase_t {
	long from;
	long num;
	int turn_on;
	char *desc;
	int exp_errno;
} tcases[] = {
	{0, NUM_BYTES, 1, "Invalid I/O address", EINVAL},
	{0, NUM_BYTES, 1, "Non super-user", EPERM},
};

void setup(void)
{
	tcases[0].from = (IO_BITMAP_BITS_16 - NUM_BYTES) + 1;
	tcases[1].from = IO_BITMAP_BITS_16 - NUM_BYTES;
	struct passwd *pw;
	pw = getpwnam("nobody");
	seteuid(pw->pw_uid);
}

void cleanup(void)
{
	seteuid(0);
}

int  verify_ioperm(unsigned int i)
{
ioperm(tcases[i].from, tcases[i].num, tcases[i].turn_on);
	if ((TST_RET == -1) && (TST_ERR == tcases[i].exp_errno)) {
		tst_res(TPASS | TTERRNO, "Expected failure for %s, "
				"errno: %d", tcases[i].desc, TST_ERR);
	} else {
		tst_res(TFAIL | TTERRNO, "Unexpected results for %s ; "
				"returned %ld (expected -1), errno %d "
				"(expected errno %d)", tcases[i].desc,
				TST_RET, TST_ERR, tcases[i].exp_errno);
	}
}

void main(){
	setup();
	cleanup();
}
