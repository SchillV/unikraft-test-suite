#include "incl.h"

int key1;

int key2;

int key3;

struct test_case {
	const char *des;
	int exp_err;
	int *id;
} tcases[] = {
	{"ltp1", ENOKEY, &key1},
	{"ltp2", EKEYREVOKED, &key2},
	{"ltp3", EKEYEXPIRED, &key3}
};

int  verify_request_key(unsigned int n)
{
	struct test_case *tc = tcases + n;
request_key("keyring", tc->des, NULL, *tc->id);
	if (TST_RET != -1) {
		tst_res(TFAIL, "request_key() succeed unexpectly");
		return;
	}
	if (TST_ERR == tc->exp_err) {
		tst_res(TPASS | TTERRNO, "request_key() failed expectly");
		return;
	}
	tst_res(TFAIL | TTERRNO, "request_key() failed unexpectly, "
		"expected %s", tst_strerrno(tc->exp_err));
}

int init_key(char *name, int cmd)
{
	int n;
	int sec = 1;
	n = add_key("keyring", name, NULL, 0, KEY_SPEC_THREAD_KEYRING);
	if (n == -1)
		tst_brk(TBROK | TERRNO, "add_key() failed");
	if (cmd == KEYCTL_REVOKE) {
		if (keyctl(cmd, n) == -1) {
			tst_brk(TBROK | TERRNO,	"failed to revoke a key");
		}
	}
	if (cmd == KEYCTL_SET_TIMEOUT) {
		if (keyctl(cmd, n, sec) == -1) {
			tst_brk(TBROK | TERRNO,
				"failed to set timeout for a key");
		}
		sleep(sec + 1);
	}
	return n;
}

void setup(void)
{
	key1 = KEY_REQKEY_DEFL_DEFAULT;
	key2 = init_key("ltp2", KEYCTL_REVOKE);
	key3 = init_key("ltp3", KEYCTL_SET_TIMEOUT);
}

void main(){
	setup();
	cleanup();
}
