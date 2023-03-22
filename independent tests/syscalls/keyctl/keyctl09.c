#include "incl.h"
#define ENCRYPTED_KEY_VALID_PAYLOAD	"new enc32 user:masterkey 32 abcdefABCDEF1234567890aaaaaaaaaaabcdefABCDEF1234567890aaaaaaaaaa"
#define ENCRYPTED_KEY_INVALID_PAYLOAD	"new enc32 user:masterkey 32 plaintext123@123!123@123!123@123plaintext123@123!123@123!123@123"

void do_test(void)
{
	char buffer[128];
	TST_EXP_POSITIVE(add_key("user", "user:masterkey", "foo", 3,
			    KEY_SPEC_PROCESS_KEYRING));
	if (!TST_PASS)
		return;
	TST_EXP_POSITIVE(add_key("encrypted", "ltptestkey1",
			    ENCRYPTED_KEY_VALID_PAYLOAD,
			    strlen(ENCRYPTED_KEY_VALID_PAYLOAD),
			    KEY_SPEC_PROCESS_KEYRING));
	if (!TST_PASS)
		return;
	TST_EXP_POSITIVE(keyctl(KEYCTL_READ, TST_RET, buffer, sizeof(buffer)));
	if (!TST_PASS)
		return;
	TST_EXP_FAIL2(add_key("encrypted", "ltptestkey2",
			    ENCRYPTED_KEY_INVALID_PAYLOAD,
			    strlen(ENCRYPTED_KEY_INVALID_PAYLOAD),
			    KEY_SPEC_PROCESS_KEYRING), EINVAL);
	keyctl(KEYCTL_CLEAR, KEY_SPEC_PROCESS_KEYRING);
}

void main(){
	setup();
	cleanup();
}
