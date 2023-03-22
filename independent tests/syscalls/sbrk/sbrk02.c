#include "incl.h"
#define INC 16*1024*1024
char *TCID = "sbrk02";
int TST_TOTAL = 1;

void setup(void);

int  sbrk_verify(

void cleanup(void);

long increment = INC;
int main(int argc, char *argv[])
{
	int lc;
	int i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			sbrk_verify();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	void *ret = NULL;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	while (ret != (void *)-1 && increment > 0) {
		ret = sbrk(increment);
		increment += INC;
	}
	tst_resm(TINFO | TERRNO, "setup() bailing inc: %ld, ret: %p, sbrk: %p",
		increment, ret, sbrk(0));
	errno = 0;
}

int  sbrk_verify(
{
	void *tret;
	tret = sbrk(increment);
	TEST_ERRNO = errno;
	if (tret != (void *)-1) {
		tst_resm(TFAIL,
			 "sbrk(%ld) returned %p, expected (void *)-1, errno=%d",
			 increment, tret, ENOMEM);
		return;
	}
	if (TEST_ERRNO == ENOMEM) {
		tst_resm(TPASS | TTERRNO, "sbrk(%ld) failed as expected",
			 increment);
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "sbrk(%ld) failed unexpectedly; expected: %d - %s",
			 increment, ENOMEM, strerror(ENOMEM));
	}
}

void cleanup(void)
{
}

