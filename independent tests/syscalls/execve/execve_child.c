#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(int argc, char *argv[])
{
	if (argc > 1 && !strcmp(argv[1], "canary")) {
		printf("argv[1] is %s, expected 'canary'\ntest succeeded\n", argv[1]);
		return 0;
	}
	printf("%s shouldn't be executed\n", argv[0]);
	return 0;
}

