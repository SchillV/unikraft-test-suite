#include "incl.h"
#define ADJ_ADJTIME 0x8000
#define LOOPS 10

struct timex *buf;
int  verify_adjtimex(
{
	int i;
	int data_leak = 0;
	for (i = 0; i < LOOPS; i++) {
		memset(buf, 0, sizeof(struct timex));
adjtimex(buf);
		if ((TST_RET == -1) && (TST_ERR == EINVAL)) {
			tst_res(TINFO,
				"expecting adjtimex() to fail with EINVAL with mode 0x%x",
				ADJ_ADJTIME);
		} else {
			tst_brk(TBROK | TERRNO,
				"adjtimex(): Unexpeceted error, expecting EINVAL with mode 0x%x",
				ADJ_ADJTIME);
		}
		tst_res(TINFO, "tai : 0x%08x", buf->tai);
		if (buf->tai != 0) {
			data_leak = 1;
			break;
		}
	}
	if (data_leak != 0)
		tst_res(TFAIL, "Data leak observed");
	else
		tst_res(TPASS, "Data leak not observed");
}

void main(){
	setup();
	cleanup();
}
