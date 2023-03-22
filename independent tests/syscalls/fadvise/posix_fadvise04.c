#include "incl.h"

int pipedes[2];

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
	     (pipedes[0], 0, 0, defined_advise[n]));
	if (TST_RET == 0) {
		tst_res(TFAIL, "call succeeded unexpectedly");
		return;
	}
	   "On error, an error number is returned." */
	if (TST_RET == ESPIPE) {
		tst_res(TPASS, "expected failure - "
			"returned value = %ld : %s",
			TST_RET, tst_strerrno(TST_RET));
	} else {
		tst_res(TFAIL,
			"unexpected return value - %ld : %s - "
			"expected %d", TST_RET,
			tst_strerrno(TST_RET), ESPIPE);
	}
}
void setup(void)
{
	pipe(pipedes);
	close(pipedes[1]);
}
void cleanup(void)
{
	if (pipedes[0] > 0)
		close(pipedes[0]);
}

void main(){
	setup();
	cleanup();
}
