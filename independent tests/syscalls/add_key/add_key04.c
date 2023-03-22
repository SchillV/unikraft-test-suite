#include "incl.h"
#define ASSOC_ARRAY_FAN_OUT 16
#define PAYLOAD "payload"

char *payload;

void do_test(void)
{
	int status;
keyctl(KEYCTL_JOIN_SESSION_KEYRING, NULL);
	if (TST_RET < 0)
		tst_brk(TBROK | TTERRNO, "failed to join new session keyring");
	if (fork() == 0) {
		char description[32];
		int i;
		for (i = 0; i < ASSOC_ARRAY_FAN_OUT; i++) {
			sprintf(description, "keyring%d", i);
add_key("keyring", description, NULL, ;
				     KEY_SPEC_SESSION_KEYRING));
			if (TST_RET < 0) {
				tst_brk(TBROK | TTERRNO,
					"unable to create keyring %d", i);
			}
		}
add_key("user", "userkey", payload, sizeof(PAYLOAD;
			     KEY_SPEC_SESSION_KEYRING));
		if (TST_RET < 0)
			tst_brk(TBROK | TTERRNO, "unable to create user key");
		exit(0);
	}
	wait(&status);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		tst_res(TPASS, "didn't crash while filling keyring");
	else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL)
		tst_res(TFAIL, "kernel oops while filling keyring");
	else
		tst_brk(TBROK, "Child %s", tst_strstatus(status));
}

void setup(void)
{
	payload = tst_strdup(PAYLOAD);
}

void main(){
	setup();
	cleanup();
}
