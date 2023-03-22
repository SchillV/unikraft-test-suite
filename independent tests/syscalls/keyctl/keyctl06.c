#include "incl.h"

void add_test_key(const char *description)
{
add_key("user", description, "payload", ;
		     KEY_SPEC_PROCESS_KEYRING));
	if (TST_RET < 0)
		tst_brk(TBROK | TTERRNO, "Failed to add test key");
}

void do_test(void)
{
	key_serial_t key_ids[2];
	add_test_key("key1");
	add_test_key("key2");
	memset(key_ids, 0, sizeof(key_ids));
keyctl(KEYCTL_READ, KEY_SPEC_PROCESS_KEYRIN;
		    (char *)key_ids, sizeof(key_serial_t)));
	if (TST_RET < 0)
		tst_brk(TBROK | TTERRNO, "KEYCTL_READ failed");
	 * Do not check key_ids[0], as the contents of the buffer are
	 * unspecified if it was too small.  However, key_ids[1] must not have
	 * been written to, as it was outside the buffer.
	 */
	if (key_ids[1] != 0)
		tst_brk(TFAIL, "KEYCTL_READ overran the buffer");
	if (TST_RET != sizeof(key_ids)) {
		tst_brk(TFAIL, "KEYCTL_READ returned %ld but expected %zu",
			TST_RET, sizeof(key_ids));
	}
	tst_res(TPASS,
		"KEYCTL_READ returned full count but didn't overrun the buffer");
}

void main(){
	setup();
	cleanup();
}
