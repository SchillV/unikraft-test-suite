#include "incl.h"
#define MAX_LINE_LENGTH 256
#define ENV_VAL "LTP test variable value"
#define ENV_ID "LTP_CLONE_TEST"

void *child_stack;

int child_environ(void *arg LTP_ATTRIBUTE_UNUSED)
{
	const char *env_val = getenv(ENV_ID);
	if (!env_val) {
		tst_res(TFAIL, "Variable " ENV_ID " not defined in child");
		exit(0);
	}
	if (strcmp(env_val, ENV_VAL)) {
		tst_res(TFAIL, "Variable value is different");
		exit(0);
	}
	tst_res(TPASS, "The environment variables of the child and the parent are the same ");
	exit(0);
}

int  verify_clone(
{
	TST_EXP_PID_SILENT(ltp_clone(SIGCHLD, child_environ, NULL, CHILD_STACK_SIZE,
				child_stack));
	if (!TST_PASS)
		return;
	tst_reap_children();
}

void setup(void)
{
	setenv(ENV_ID, ENV_VAL, 0);
}

void main(){
	setup();
	cleanup();
}
