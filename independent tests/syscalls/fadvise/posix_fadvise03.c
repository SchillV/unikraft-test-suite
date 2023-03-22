#include "incl.h"
int expected_error = EINVAL;
int defined_advise[] = {
	POSIX_FADV_NORMAL,
	POSIX_FADV_SEQUENTIAL,
	POSIX_FADV_RANDOM,
	POSIX_FADV_WILLNEED,
#if defined(__s390__) && defined(TST_ABI32)
	 * but the kernel accepts 4,5 as well and rewrites them internally,
	 * see Linux kernel commit 068e1b94bbd268f375349f68531829c8b7c210bc
	 *
	 * since header definitions are incomplete - posix fcntl.h doesn't care
	 * and defines them as 4,5 while linux/fadvise.h (which uses 6,7)
	 * matches only 64bit - we need to hardcode the values here for
	 * all 4 cases, unfortunately
	 */
#else
	POSIX_FADV_DONTNEED,
	POSIX_FADV_NOREUSE,
#endif
};
const int defined_advise_total = ARRAY_SIZE(defined_advise);
#define ADVISE_LIMIT 32
   Return 1 if advise is in defined_advise.
   Return 0 if not. */

int is_defined_advise(int advise)
{
	int i;
	for (i = 0; i < defined_advise_total; i++) {
		if (defined_advise[i] == advise)
			return 1;
	}
	return 0;
}

int  verify_fadvise(unsigned int n)
{
	if (is_defined_advise(n)) {
		tst_res(TPASS, "skipping defined - advise = %d", n);
		return;
	}
posix_fadvise(fd, 0, 0, n);
	if (TST_RET == 0) {
		tst_res(TFAIL, "call succeeded unexpectedly");
		return;
	}
	   "On error, an error number is returned." */
	if (TST_RET == expected_error) {
		tst_res(TPASS,
			"expected failure - "
			"returned value = %ld, advise = %d : %s",
			TST_RET,
			n, tst_strerrno(TST_RET));
	} else {
		tst_res(TFAIL,
			"unexpected return value - %ld : %s, advise %d - "
			"expected %d",
			TST_RET,
			tst_strerrno(TST_RET),
			n, expected_error);
	}
}

void setup(void)
{
	fd = open(fname, O_RDONLY);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
