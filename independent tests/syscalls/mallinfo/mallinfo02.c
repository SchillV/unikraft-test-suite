#include "incl.h"
#ifdef HAVE_MALLINFO
void test_mallinfo(void)
{
	struct mallinfo info;
	int size;
	char *buf;
	buf = malloc(20480);
	info = mallinfo();
	if (info.uordblks > 20480 && info.hblkhd == 0) {
		tst_res(TPASS, "malloc() uses sbrk when size < 128k");
	} else {
		tst_res(TFAIL, "malloc() use mmap when size < 128k");
		print_mallinfo("Test malloc(20480)", &info);
	}
	free(buf);
	info = mallinfo();
	size = MAX(info.fordblks, 131072);
	buf = malloc(size);
	info = mallinfo();
	if (info.hblkhd > size && info.hblks > 0) {
		tst_res(TPASS, "malloc() uses mmap when size >= 128k");
	} else {
		tst_res(TFAIL, "malloc uses sbrk when size >= 128k");
		print_mallinfo("Test malloc(1024*128)", &info);
	}
	free(buf);
}

void setup(void)
{
	if (mallopt(M_MMAP_THRESHOLD, 131072) == 0)
		tst_res(TFAIL, "mallopt(M_MMAP_THRESHOLD, 128K) failed");
}

void main(){
	setup();
	cleanup();
}
