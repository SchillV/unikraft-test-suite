#include "incl.h"
#define BSIZE	4096
#define LEN	100
char buf[BSIZE];

void clearit(void)
{
	register int i;
	for (i = 0; i < BSIZE; i++)
		buf[i] = 0;
}

void fill(char *str, int len)
{
	register int i;
	for (i = 0; i < len; i++)
		*str++ = 'a';
}

int checkit(char *str, int len)
{
	register int i;
	for (i = 0; i < len; i++)
		if (*str++ != 'a')
			return (-1);
	return 0;
}

struct test_case {
	char *p;
	char *q;
	int len;
} tcases[] = {
	{&buf[100], &buf[800], LEN},
	{&buf[800], &buf[100], LEN},
};

void setup(void)
{
	clearit();
	return;
}

int  verify_memcpy(char *p, char *q, int len)
{
	fill(p, len);
	memcpy(q, p, LEN);
	if (checkit(q, len)) {
		tst_res(TFAIL, "copy failed - missed data");
		goto out;
	}
	if (p[-1] || p[LEN]) {
		tst_res(TFAIL, "copy failed - 'to' bounds");
		goto out;
	}
	if (q[-1] || q[LEN]) {
		tst_res(TFAIL, "copy failed - 'from' bounds");
		goto out;
	}
	tst_res(TPASS, "Test passed");
out:
	return;
}

void run_test(unsigned int nr)
{
	struct test_case *tc = &tcases[nr];
	clearit();
int 	verify_memcpy(tc->p, tc->q, tc->len);
	return;
}

void main(){
	setup();
	cleanup();
}
