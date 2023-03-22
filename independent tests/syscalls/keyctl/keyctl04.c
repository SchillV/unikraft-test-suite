#include "incl.h"

void do_test(void)
{
	key_serial_t tid_keyring;
keyctl(KEYCTL_GET_KEYRING_ID, KEY_SPEC_THREAD_KEYRING, 1);
	if (TST_RET < 0)
		tst_brk(TBROK | TTERRNO, "failed to create thread keyring");
	tid_keyring = TST_RET;
keyctl(KEYCTL_SET_REQKEY_KEYRING, KEY_REQKEY_DEFL_THREAD_KEYRING);
	if (TST_RET < 0)
		tst_brk(TBROK | TTERRNO, "failed to set reqkey keyring");
keyctl(KEYCTL_GET_KEYRING_ID, KEY_SPEC_THREAD_KEYRING, 0);
	if (TST_RET < 0)
		tst_brk(TBROK | TTERRNO, "failed to get thread keyring ID");
	if (TST_RET == tid_keyring)
		tst_res(TPASS, "thread keyring was not leaked");
	else
		tst_res(TFAIL, "thread keyring was leaked!");
}

void main(){
	setup();
	cleanup();
}
