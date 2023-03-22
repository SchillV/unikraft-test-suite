#include "incl.h"
int expected_return = 0;
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
posix_fadvise(fd, 0, 0, defined_advise[n]);
	   "On error, an error number is returned." */
	if (TST_RET == expected_return) {
		tst_res(TPASS, "call succeeded expectedly");
	} else {
		tst_res(TFAIL,
			"unexpected return value - %ld : %s, advise %d - "
			"expected %d",
			TST_RET,
			tst_strerrno(TST_RET),
			defined_advise[n], expected_return);
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
