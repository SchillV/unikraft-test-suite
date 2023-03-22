#include "incl.h"
void run(void)
{
	if (keyctl_join_session_keyring(".builtin_trusted_keys") == -1) {
		if (errno != EPERM) {
			tst_brk(TBROK | TERRNO,
				"keyctl_join_sessoin_keyring(...)");
		}
		tst_res(TPASS, "Denied access to .builtin_trusted_keys");
	} else {
		tst_res(TFAIL, "Allowed access to .builtin_trusted_keys");
	}
}

void main(){
	setup();
	cleanup();
}
