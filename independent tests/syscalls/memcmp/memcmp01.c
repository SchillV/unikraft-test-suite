#include "incl.h"
#define BSIZE	4096
#define LEN	100
char buf[BSIZE];

struct test_case {
	char *p;
	char *q;
	int len;
} tcases[] = {
	{&buf[100], &buf[800], LEN},
	{&buf[800], &buf[100], LEN},
};

void fill(char *str, int len)
{
	register int i;
	for (i = 0; i < len; i++)
		*str++ = 'a';
}

void setup(void)
{
	register int i;
	for (i = 0; i < BSIZE; i++)
		buf[i] = 0;
	return;
}

int  verify_memcmp(char *p, char *q, int len)
{
	fill(p, len);
	fill(q, len);
	if (memcmp(p, q, len)) {
		tst_res(TFAIL, "memcmp fails - should have succeeded.");
		goto out;
	}
	p[len - 1] = 0;
	if (memcmp(p, q, len) >= 0) {
		tst_res(TFAIL, "memcmp succeeded - should have failed.");
		goto out;
	};
	p[len - 1] = 'a';
	p[0] = 0;
	if (memcmp(p, q, len) >= 0) {
		tst_res(TFAIL, "memcmp succeeded - should have failed.");
		goto out;
	};
	p[0] = 'a';
	q[len - 1] = 0;
	if (memcmp(p, q, len) <= 0) {
		tst_res(TFAIL, "memcmp succeeded - should have failed.");
		goto out;
	};
	q[len - 1] = 'a';
	q[0] = 0;
	if (memcmp(p, q, len) <= 0) {
		tst_res(TFAIL, "memcmp succeeded - should have failed.");
		goto out;
	};
	q[0] = 'a';
	if (memcmp(p, q, len)) {
		tst_res(TFAIL, "memcmp fails - should have succeeded.");
		goto out;
	}
	tst_res(TPASS, "Test passed");
out:
	return;
}

void run_test(unsigned int nr)
{
	struct test_case *tc = &tcases[nr];
int 	verify_memcmp(tc->p, tc->q, tc->len);
	return;
}

void main(){
	setup();
	cleanup();
}
