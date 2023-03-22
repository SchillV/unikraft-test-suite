#include "incl.h"
				   Just used as something wrong fd */
int defined_advise[] = {
	POSIX_FADV_NORMAL,
	POSIX_FADV_SEQUENTIAL,
	POSIX_FADV_RANDOM,
	POSIX_FADV_NOREUSE,
	POSIX_FADV_WILLNEED,
	POSIX_FADV_DONTNEED,
};

int  verify_fadvise(unsigned int n)
{
posix_fadvi;
	     (WRONG_FD, 0, 0, defined_advise[n]));
	if (TST_RET == 0) {
		tst_res(TFAIL, "call succeeded unexpectedly");
		return;
	}
	   "On error, an error number is returned." */
	if (TST_RET == EBADF) {
		tst_res(TPASS, "expected failure - "
			"returned value = %ld : %s",
			TST_RET, tst_strerrno(TST_RET));
	} else {
		tst_res(TFAIL,
			"unexpected returnd value - %ld : %s - "
			"expected %d", TST_RET,
			tst_strerrno(TST_RET), EBADF);
	}
}

void setup(void)
{
retry:
	errno = 0;
	if (close(WRONG_FD) != 0) {
		if (errno == EINTR)
			goto retry;
		else if (errno == EIO)
			tst_brk(TBROK,
				"Unable to close a file descriptor(%d): %s\n",
				WRONG_FD, tst_strerrno(EIO));
	}
}

void main(){
	setup();
	cleanup();
}
