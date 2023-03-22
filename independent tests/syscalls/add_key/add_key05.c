#include "incl.h"
#define MAX_USERS 10

char *user_buf;

uid_t ltpuser[MAX_USERS];

unsigned int usern;

unsigned int useri;

void add_user(char n)
{
	char username[] = "ltp_add_key05_n";
	const char *const cmd_useradd[] = {"useradd", username, NULL};
	const char *const cmd_userdel[] = {"userdel", "-r", username, NULL};
	const char *const cmd_groupdel[] = {"groupdel", username, NULL};
	struct passwd *pw;
	username[sizeof(username) - 2] = '0' + n;
	tst_cmd(cmd_userdel, NULL, "/dev/null", TST_CMD_PASS_RETVAL);
	tst_cmd(cmd_groupdel, NULL, "/dev/null", TST_CMD_PASS_RETVAL);
	cmd(cmd_useradd, NULL, NULL);
	pw = getpwnam(username);
	ltpuser[(unsigned int)n] = pw->pw_uid;
	tst_res(TINFO, "Created user %s", pw->pw_name);
}

void clean_user(char n)
{
	char username[] = "ltp_add_key05_n";
	const char *const cmd_userdel[] = {"userdel", "-r", username, NULL};
	username[sizeof(username) - 2] = '0' + n;
	if (tst_cmd(cmd_userdel, NULL, NULL, TST_CMD_PASS_RETVAL))
		tst_res(TWARN | TERRNO, "'userdel -r %s' failed", username);
}

inline void parse_proc_key_users(int *used_key, int *max_key, int *used_bytes, int *max_bytes)
{
	unsigned int val[4];
	char fmt[1024];
	sprintf(fmt, "%5u: %%*5d %%*d/%%*d %%d/%%d %%d/%%d", ltpuser[useri]);
	file_lines_scanf("/proc/key-users", fmt, &val[0], &val[1], &val[2], &val[3]);
	if (used_key)
		*used_key = val[0];
	if (max_key)
		*max_key = val[1];
	if (used_bytes)
		*used_bytes = val[2];
	if (max_bytes)
		*max_bytes = val[3];
}

int  verify_max_bytes(
{
	char *buf;
	int plen, invalid_plen, delta;
	int used_bytes, max_bytes, tmp_used_bytes;
	tst_res(TINFO, "test max bytes under unprivileged user");
	parse_proc_key_users(NULL, NULL, &tmp_used_bytes, NULL);
add_key("user", "test2", user_buf, 64, KEY_SPEC_THREAD_KEYRING);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "add key test2 failed");
		return;
	}
	parse_proc_key_users(NULL, NULL, &used_bytes, &max_bytes);
	 * Compute delta between default datalen(in key_alloc) and actual
	 * datlen(in key_payload_reserve).
	 * more info see kernel code: security/keys/key.c
	 */
	delta = used_bytes - tmp_used_bytes - strlen("test2") - 1 - 64;
	invalid_plen = max_bytes - used_bytes - delta - strlen("test_xxx");
	buf = tst_alloc(invalid_plen);
add_key("user", "test_inv", buf, invalid_plen, KEY_SPEC_THREAD_KEYRING);
	if (TST_RET != -1) {
		tst_res(TFAIL, "add_key(test_inv) succeeded unexpectedltly");
		return;
	}
	if (TST_ERR == EDQUOT)
		tst_res(TPASS | TTERRNO, "add_key(test_inv) failed as expected");
	else
		tst_res(TFAIL | TTERRNO, "add_key(test_inv) failed expected EDQUOT got");
add_key("user", "test3", user_buf, 64, KEY_SPEC_THREAD_KEYRING);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "add key test3 failed");
		return;
	}
add_key("user", "test4", user_buf, 64, KEY_SPEC_THREAD_KEYRING);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "add key test4 failed");
		return;
	}
	parse_proc_key_users(NULL, NULL, &used_bytes, &max_bytes);
	plen = max_bytes - used_bytes - delta - strlen("test_xxx") - 1;
add_key("user", "test_max", buf, plen, KEY_SPEC_THREAD_KEYRING);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "add_key(test_max) failed unexpectedly");
		return;
	}
	tst_res(TPASS, "add_key(test_max) succeeded as expected");
	parse_proc_key_users(NULL, NULL, &tmp_used_bytes, &max_bytes);
	if (tmp_used_bytes == max_bytes)
		tst_res(TPASS, "allow reaching the max bytes exactly");
	else
		tst_res(TFAIL, "max used bytes %u, key allow max bytes %u", tmp_used_bytes, max_bytes);
}

int  verify_max_keys(
{
	int i, used_key, max_key;
	char desc[10];
	tst_res(TINFO, "test max keys under unprivileged user");
	parse_proc_key_users(&used_key, &max_key, NULL, NULL);
	for (i = used_key + 1; i <= max_key; i++) {
		sprintf(desc, "abc%d", i);
add_key("user", desc, user_buf, 64, KEY_SPEC_THREAD_KEYRING);
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO, "add keyring key(%s) failed", desc);
			goto count;
		}
	}
add_key("user", "test_invalid_key", user_buf, 64, KEY_SPEC_THREAD_KEYRING);
	if (TST_RET != -1) {
		tst_res(TFAIL, "add keyring key(test_invalid_key) succeeded unexpectedly");
		goto count;
	}
	if (TST_ERR == EDQUOT)
		tst_res(TPASS | TTERRNO, "add_key(test_invalid_key) failed as expected");
	else
		tst_res(TFAIL | TTERRNO, "add_key(test_invalid_key) failed expected EDQUOT got");
count:
	parse_proc_key_users(&used_key, &max_key, NULL, NULL);
	if (used_key == max_key)
		tst_res(TPASS, "allow reaching the max key(%u) exactly", max_key);
	else
		tst_res(TFAIL, "max used key %u, key allow max key %u", used_key, max_key);
}

void do_test(unsigned int n)
{
	if (usern < MAX_USERS)
		add_user(usern++);
	if (useri >= MAX_USERS) {
		sleep(1);
		useri = 0;
	}
	if (!fork()) {
		setuid(ltpuser[useri]);
		tst_res(TINFO, "User: %d, UID: %d", useri, ltpuser[useri]);
add_key("user", "test1", user_buf, 64, KEY_SPEC_THREAD_KEYRING);
		if (TST_RET == -1) {
			tst_res(TFAIL | TTERRNO, "add key test1 failed");
			return;
		}
		if (n)
int 			verify_max_keys();
		else
int 			verify_max_bytes();
	}
	tst_reap_children();
	useri++;
	return;
}

void cleanup(void)
{
	while (usern--)
		clean_user(usern);
}

void main(){
	setup();
	cleanup();
}
