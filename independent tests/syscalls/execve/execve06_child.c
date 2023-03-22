#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(int argc, char *argv[])
{
	if (argc != 1) {
		printf("argc is %d, expected 1\n", argc);
		return 0;
	}
	if (!argv[0]) {
		printf("argv[0] == NULL\n");
		return 0;
	}
	printf("argv[0] was filled in by kernel\n");
	return 0;
}

