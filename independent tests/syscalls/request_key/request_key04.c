#include "incl.h"

void do_test(void)
{
	key_serial_t keyid;
	int saved_errno;
keyctl(KEYCTL_JOIN_SESSION_KEYRING, NULL);
	if (TST_RET < 0)
		tst_brk(TBROK | TTERRNO, "failed to join new session keyring");
keyctl(KEYCTL_SETPERM, KEY_SPEC_SESSION_KEYRIN;
		    KEY_POS_SEARCH|KEY_POS_READ|KEY_POS_VIEW));
	if (TST_RET < 0) {
		tst_brk(TBROK | TTERRNO,
			"failed to set permissions on session keyring");
	}
keyctl(KEYCTL_SET_REQKEY_KEYRIN;
		    KEY_REQKEY_DEFL_SESSION_KEYRING));
	if (TST_RET < 0) {
		tst_brk(TBROK | TTERRNO,
			"failed to set request-key default keyring");
	}
keyctl(KEYCTL_READ, KEY_SPEC_SESSION_KEYRIN;
		    &keyid, sizeof(keyid)));
	if (TST_RET < 0)
		tst_brk(TBROK | TTERRNO, "failed to read from session keyring");
	if (TST_RET != 0)
		tst_brk(TBROK, "session keyring is not empty");
request_key("user", "desc", "callout_info", 0);
	if (TST_RET != -1)
		tst_brk(TBROK, "request_key() unexpectedly succeeded");
	saved_errno = TST_ERR;
keyctl(KEYCTL_READ, KEY_SPEC_SESSION_KEYRIN;
		    &keyid, sizeof(keyid)));
	if (TST_RET < 0)
		tst_brk(TBROK | TTERRNO, "failed to read from session keyring");
	if (TST_RET != 0)
		tst_brk(TFAIL, "added key to keyring without permission");
	TST_ERR = saved_errno;
	if (TST_ERR == EACCES) {
		tst_res(TPASS, "request_key() failed with EACCES as expected");
	} else {
		tst_res(TFAIL | TTERRNO,
			"request_key() failed with unexpected error code");
	}
}

void main(){
	setup();
	cleanup();
}
