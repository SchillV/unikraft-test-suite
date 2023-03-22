#include "incl.h"

key_serial_t create_keyring(const char *description)
{
add_key("keyring", description, NULL, ;
		     KEY_SPEC_PROCESS_KEYRING));
	if (TST_RET < 0) {
		tst_brk(TBROK | TTERRNO,
			"unable to create keyring '%s'", description);
	}
	return TST_RET;
}

key_serial_t get_keyring_id(key_serial_t special_id)
{
keyctl(KEYCTL_GET_KEYRING_ID, special_id, 1);
	if (TST_RET < 0) {
		tst_brk(TBROK | TTERRNO,
			"unable to get ID of keyring %d", special_id);
	}
	return TST_RET;
}

void do_test(void)
{
	uid_t uid = 1;
	char description[32];
	key_serial_t fake_user_keyring;
	key_serial_t fake_user_session_keyring;
	 * We need a user to forge the keyrings for.  But the bug is not
	 * reproducible for a UID which already has its keyrings, so find an
	 * unused UID.  Note that it would be better to directly check for the
	 * presence of the UID's keyrings than to search the passwd file.
	 * However, that's not easy to do given that even if we assumed the UID
	 * temporarily to check, KEYCTL_GET_KEYRING_ID for the user and user
	 * session keyrings will create them rather than failing (even if the
	 * 'create' argument is 0).
	 */
	while (getpwuid(uid))
		uid++;
	sprintf(description, "_uid.%u", uid);
	fake_user_keyring = create_keyring(description);
	sprintf(description, "_uid_ses.%u", uid);
	fake_user_session_keyring = create_keyring(description);
	setuid(uid);
	if (fake_user_keyring == get_keyring_id(KEY_SPEC_USER_KEYRING))
		tst_brk(TFAIL, "created user keyring for another user");
	if (fake_user_session_keyring ==
	    get_keyring_id(KEY_SPEC_USER_SESSION_KEYRING))
		tst_brk(TFAIL, "created user session keyring for another user");
	tst_res(TPASS, "expectedly could not create another user's keyrings");
}

void main(){
	setup();
	cleanup();
}
