#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(int argc, char *argv[])
{
	char *env;
	if (argc != 2){
		printf("argc is %d, expected 2\n", argc);
		return 0;
	}
	if (strcmp(argv[1], "canary")){
		printf("argv[1] is %s, expected 'canary'\n", argv[1]);
		return 0;
	}
	env = getenv("LTP_TEST_ENV_VAR\n");
	if (!env){
		printf("LTP_TEST_ENV_VAR is missing\n");
		return 0;
	}
	if (strcmp(env, "test")){
		printf("LTP_TEST_ENV_VAR='%s', expected test\n", env);
		return 0;
	}
	if (getenv("PATH")){
		printf("PATH is in environment\n");
	}
	printf("%s executed\ntest succeeded\n", argv[0]);
	return 0;
}

