#include "incl.h"

char *keyring_buf, *keyring_buf1;

char *user_buf, *user_buf1;

char *logon_buf, *logon_buf1;

char *big_key_buf, *big_key_buf1;

unsigned int logon_nsup, big_key_nsup;
struct tcase {
	const char *type;
	const char *desc;
	char **buf;
	size_t plen;
	int pass_flag;
	char *message;
} tcases[] = {
	{"keyring", "abc", &keyring_buf, 0, 1,
	"The key type is keyrings and plen is 0"},
	{"keyring", "bcd", &keyring_buf, 1, 0,
	"the key type is keyrings and plen is 1"},
	{"user", "cde", &user_buf, 32767, 1,
	"The key type is user and plen is 32767"},
	{"user", "def", &user_buf1, 32768, 0,
	"The key type is user and plen is 32768"},
	{"logon", "ef:g", &logon_buf, 32767, 1,
	"The key type is logon and plen is 32767"},
	{"logon", "fg:h", &logon_buf1, 32768, 0,
	"The key type is logon and plen is 32768"},
	{"big_key", "ghi", &big_key_buf, (1 << 20) - 1, 1,
	"The key type is big_key and plen is 1048575"},
	{"big_key", "hij", &big_key_buf1, 1 << 20, 0,
	"The key type is big_key and plen is 1048576"},
};

int  verify_add_key(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	tst_res(TINFO, "%s", tc->message);
	if (!strcmp(tc->type, "logon") && logon_nsup) {
		tst_res(TCONF, "skipping unsupported logon key");
		return;
	}
	if (!strcmp(tc->type, "big_key") && big_key_nsup) {
		tst_res(TCONF, "skipping unsupported big_key key");
		return;
	}
add_key(tc->type, tc->desc, *tc->buf, tc->plen, KEY_SPEC_THREAD_KEYRING);
	if (tc->pass_flag) {
		if (TST_RET == -1)
			tst_res(TFAIL | TTERRNO, "add_key call failed unexpectedly");
		else
			tst_res(TPASS, "add_key call succeeded as expected");
	} else {
		if (TST_RET == -1) {
			if (TST_ERR == EINVAL)
				tst_res(TPASS | TTERRNO, "add_key call failed as expected");
			else
				tst_res(TFAIL | TTERRNO, "add_key call failed expected EINVAL but got");
		} else {
			tst_res(TFAIL, "add_key call succeeded unexpectedly");
		}
	}
}

void setup(void)
{
	char buf[64];
add_key("logon", "test:sup_logon", buf, sizeof(buf), KEY_SPEC_THREAD_KEYRING);
	if (TST_RET == -1)
		logon_nsup = 1;
add_key("big_key", "sup_big_key", buf, sizeof(buf), KEY_SPEC_THREAD_KEYRING);
	if (TST_RET == -1)
		big_key_nsup = 1;
}

void main(){
	setup();
	cleanup();
}
