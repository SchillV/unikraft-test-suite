#include "incl.h"
#ifdef HAVE_MALLINFO
#define M_NUM 20

struct mallinfo info1;

void *buf[M_NUM + 1];

void cleanup(void)
{
	int i;
	for (i = M_NUM; i > 0; i--) {
		if (buf[i]) {
			free(buf[i]);
			buf[i] = NULL;
		}
	}
}
void test_mallinfo(void)
{
	int i;
	int total = 0;
	struct mallinfo info2;
	for (i = 1; i <= M_NUM; i++) {
		buf[i] = malloc(160 * i);
		total += 160 * i;
	}
	info2 = mallinfo();
	print_mallinfo("Test uordblks", &info2);
	if (info2.uordblks >= info1.uordblks + total)
		tst_res(TPASS, "mallinfo() uordblks passed");
	else
		tst_res(TFAIL, "mallinfo() uordblks failed");
	free(buf[M_NUM/2]);
	buf[M_NUM/2] = NULL;
	info2 = mallinfo();
	print_mallinfo("Test ordblks", &info2);
	if (info2.ordblks == info1.ordblks + 1)
		tst_res(TPASS, "mallinfo() ordblks passed");
	else
		tst_res(TFAIL, "mallinfo() ordblks failed");
	cleanup();
}

void setup(void)
{
	if (sizeof(info1.arena) != sizeof(int))
		tst_res(TFAIL, "The member of mallinfo struct is not int");
	info1 = mallinfo();
	print_mallinfo("Start", &info1);
}

void main(){
	setup();
	cleanup();
}
