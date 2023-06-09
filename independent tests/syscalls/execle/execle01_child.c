#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(int argc, char *argv[])
{
	char *env;
	tst_reinit();
	if (argc != 2)
		tst_brk(TFAIL, "argc is %d, expected 2", argc);
	if (strcmp(argv[1], "canary"))
		tst_brk(TFAIL, "argv[1] is %s, expected 'canary'", argv[1]);
	env = getenv("LTP_TEST_ENV_VAR");
	if (!env)
		tst_brk(TFAIL, "LTP_TEST_ENV_VAR is missing");
	if (strcmp(env, "test"))
		tst_brk(TFAIL, "LTP_TEST_ENV_VAR='%s', expected test", env);
	if (getenv("PATH"))
		tst_brk(TFAIL, "PATH is in environment");
	tst_res(TPASS, "%s executed", argv[0]);
	return 0;
}

