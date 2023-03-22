#include "incl.h"
#define BSIZE 4096
char buf[BSIZE];

void fill(void)
{
	register int i;
	for (i = 0; i < BSIZE; i++)
		buf[i] = 'a';
}

int checkit(char *str)
{
	register int i = 0;
	while (!*str++)
		i++;
	return i;
}

void setup(void)
{
	fill();
}

int  verify_memset(
{
	register int i, j;
	char *p = &buf[400];
	for (i = 0; i < 200; i++) {
		fill();
		memset(p, 0, i);
		if ((j = checkit(p)) != i) {
			tst_res(TINFO, "Not enough zero bytes, wanted %d, got %d", i, j);
			break;
		}
		if (!p[-1] || !p[i]) {
			tst_res(TINFO, "Boundary error, clear of %d", i);
			break;
		}
	}
	if (i == 200)
		tst_res(TPASS, "Test passed");
	else
		tst_res(TFAIL, "Test fails");
}

void main(){
	setup();
	cleanup();
}
