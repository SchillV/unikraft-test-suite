#include "incl.h"
#ifdef HAVE_MALLOPT
#define MAX_FAST_SIZE	(80 * sizeof(size_t) / 4)
struct mallinfo info;
void test_mallopt(void)
{
	char *buf;
	buf = malloc(20480);
	info = mallinfo();
	if (info.uordblks < 20480) {
		print_mallinfo("Test uordblks", &info);
		tst_res(TFAIL, "mallinfo() failed: uordblks < 20K");
	}
	if (info.smblks != 0) {
		print_mallinfo("Test smblks", &info);
		tst_res(TFAIL, "mallinfo() failed: smblks != 0");
	}
	if (info.uordblks >= 20480 && info.smblks == 0)
		tst_res(TPASS, "mallinfo() succeeded");
	free(buf);
	if (mallopt(M_MXFAST, MAX_FAST_SIZE) == 0)
		tst_res(TFAIL, "mallopt(M_MXFAST, %d) failed", (int)MAX_FAST_SIZE);
	else
		tst_res(TPASS, "mallopt(M_MXFAST, %d) succeeded", (int)MAX_FAST_SIZE);
	if ((buf = malloc(1024)) == NULL) {
		tst_res(TFAIL, "malloc(1024) failed");
	} else {
		tst_res(TPASS, "malloc(1024) succeeded");
		free(buf);
	}
	if (mallopt(M_MXFAST, 0) == 0)
		tst_res(TFAIL, "mallopt(M_MXFAST, 0) failed");
	else
		tst_res(TPASS, "mallopt(M_MXFAST, 0) succeeded");
	if ((buf = malloc(1024)) == NULL) {
		tst_res(TFAIL, "malloc(1024) failed");
	} else {
		tst_res(TPASS, "malloc(1024) succeeded");
		free(buf);
	}
}

void main(){
	setup();
	cleanup();
}
