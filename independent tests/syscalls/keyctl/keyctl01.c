#include "incl.h"

void do_test(void)
{
	key_serial_t key;
keyctl(KEYCTL_GET_KEYRING_ID, KEY_SPEC_USER_SESSION_KEYRING);
	if (TST_RET != -1)
		tst_res(TPASS, "KEYCTL_GET_KEYRING_ID succeeded");
	else
		tst_res(TFAIL | TTERRNO, "KEYCTL_GET_KEYRING_ID failed");
	for (key = INT32_MAX; key > INT32_MIN; key--) {
keyctl(KEYCTL_READ, key);
		if (TST_RET == -1 && TST_ERR == ENOKEY)
			break;
	}
keyctl(KEYCTL_REVOKE, key);
	if (TST_RET != -1) {
		tst_res(TFAIL, "KEYCTL_REVOKE succeeded unexpectedly");
		return;
	}
	if (TST_ERR != ENOKEY) {
		tst_res(TFAIL | TTERRNO, "KEYCTL_REVOKE failed unexpectedly");
		return;
	}
	tst_res(TPASS | TTERRNO, "KEYCTL_REVOKE failed as expected");
}

void main(){
	setup();
	cleanup();
}
