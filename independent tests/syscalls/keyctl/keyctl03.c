#include "incl.h"

void do_test(void)
{
	key_serial_t key;
	key = add_key("user", "ltptestkey", "a", 1, KEY_SPEC_SESSION_KEYRING);
	if (key == -1)
		tst_brk(TBROK, "Failed to add key");
	request_key("keyring", "foo", "bar", KEY_SPEC_THREAD_KEYRING);
keyctl(KEYCTL_UNLINK, key, KEY_SPEC_SESSION_KEYRING);
	if (TST_RET)
		tst_res(TFAIL | TTERRNO, "keyctl unlink failed");
	else
		tst_res(TPASS, "Bug not reproduced");
}

void main(){
	setup();
	cleanup();
}
