#include "incl.h"

int key;

int  verify_request_key(
{
request_key("keyring", "ltp", NULL, KEY_REQKEY_DEFL_DEFAULT);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "request_key() failed");
		return;
	}
	if (TST_RET != key)
		tst_res(TFAIL, "serial number mismatched");
	else
		tst_res(TPASS, "request_key() succeed");
}

void setup(void)
{
	key = add_key("keyring", "ltp", NULL, 0, KEY_SPEC_THREAD_KEYRING);
	if (key == -1)
		tst_brk(TBROK | TERRNO, "add_key() failed");
}

void main(){
	setup();
	cleanup();
}
