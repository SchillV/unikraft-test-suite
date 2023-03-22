#include "incl.h"
void set_bit(unsigned long *b, unsigned int n, unsigned int v)
{
	if (v)
		b[n / bitsperlong] |= 1UL << (n % bitsperlong);
	else
		b[n / bitsperlong] &= ~(1UL << (n % bitsperlong));
}
int check_ret(long expected_ret)
{
	if (expected_ret == TEST_RETURN) {
		tst_resm(TPASS, "expected ret success: "
			 "returned value = %ld", TEST_RETURN);
		return 0;
	} else
		tst_resm(TFAIL, "unexpected failure - "
			 "returned value = %ld, expected: %ld",
			 TEST_RETURN, expected_ret);
	return 1;
}
int check_errno(long expected_errno)
{
	if (TEST_ERRNO == expected_errno) {
		tst_resm(TPASS | TTERRNO, "expected failure");
		return 0;
	} else if (TEST_ERRNO == 0)
		tst_resm(TFAIL, "call succeeded unexpectedly");
	else
		tst_resm(TFAIL | TTERRNO, "unexpected failure - "
			 "expected = %ld : %s, actual",
			 expected_errno, strerror(expected_errno));
	return 1;
}

void main(){
	setup();
	cleanup();
}
