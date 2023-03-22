#include "incl.h"
struct tcase {
	const char *type;
	size_t plen;
} tcases[] = {
	 * The payload length we test for each key type needs to pass initial
	 * validation but is otherwise arbitrary.  Note: the "rxrpc_s" key type
	 * requires a payload of exactly 8 bytes.
	 */
	{ "asymmetric",		64 },
	{ "cifs.idmap",		64 },
	{ "cifs.spnego",	64 },
	{ "pkcs7_test",		64 },
	{ "rxrpc",		64 },
	{ "rxrpc_s",		 8 },
	{ "user",		64 },
	{ "logon",              64 },
	{ "big_key",            64 },
};

int  verify_add_key(unsigned int i)
{
add_key(tcases[i].typ;
		"abc:def", NULL, tcases[i].plen, KEY_SPEC_PROCESS_KEYRING));
	if (TST_RET != -1) {
		tst_res(TFAIL,
			"add_key() with key type '%s' unexpectedly succeeded",
			tcases[i].type);
		return;
	}
	if (TST_ERR == EFAULT) {
		tst_res(TPASS, "received expected EFAULT with key type '%s'",
			tcases[i].type);
		return;
	}
	if (TST_ERR == ENODEV) {
		tst_res(TCONF, "kernel doesn't support key type '%s'",
			tcases[i].type);
		return;
	}
	 * It's possible for the "asymmetric" key type to be supported, but with
	 * no asymmetric key parsers registered.  In that case, attempting to
	 * add a key of type asymmetric will fail with EBADMSG.
	 */
	if (TST_ERR == EBADMSG && !strcmp(tcases[i].type, "asymmetric")) {
		tst_res(TCONF, "no asymmetric key parsers are registered");
		return;
	}
	tst_res(TFAIL | TTERRNO, "unexpected error with key type '%s'",
		tcases[i].type);
}

void main(){
	setup();
	cleanup();
}
