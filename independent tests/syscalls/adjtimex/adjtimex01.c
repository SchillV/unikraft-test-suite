#include "incl.h"
#define SET_MODE (ADJ_OFFSET | ADJ_FREQUENCY | ADJ_MAXERROR | ADJ_ESTERROR | \
	ADJ_STATUS | ADJ_TIMECONST | ADJ_TICK)

struct timex *tim_save;

struct timex *buf;
int  verify_adjtimex(
{
	buf->modes = SET_MODE;
adjtimex(buf);
	if ((TST_RET >= TIME_OK) && (TST_RET <= TIME_ERROR)) {
		tst_res(TPASS, "adjtimex() with mode 0x%x ", SET_MODE);
	} else {
		tst_res(TFAIL | TTERRNO, "adjtimex() with mode 0x%x ",
				SET_MODE);
	}
	buf->modes = ADJ_OFFSET_SINGLESHOT;
adjtimex(buf);
	if ((TST_RET >= TIME_OK) && (TST_RET <= TIME_ERROR)) {
		tst_res(TPASS, "adjtimex() with mode 0x%x ",
				ADJ_OFFSET_SINGLESHOT);
	} else {
		tst_res(TFAIL | TTERRNO,
				"adjtimex() with mode 0x%x ",
				ADJ_OFFSET_SINGLESHOT);
	}
}

void setup(void)
{
	tim_save->modes = 0;
	if ((adjtimex(tim_save)) == -1) {
		tst_brk(TBROK | TERRNO,
			"adjtimex(): failed to save current params");
	}
}

void main(){
	setup();
	cleanup();
}
