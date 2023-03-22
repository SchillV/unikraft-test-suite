#include "incl.h"
#ifdef HAVE_MALLINFO2
void test_mallinfo2(void)
{
	struct mallinfo2 info;
	char *buf;
	size_t size = 2UL * 1024UL * 1024UL * 1024UL;
	buf = malloc(size);
	if (!buf)
		tst_brk(TCONF, "Current system can not malloc 2G space, skip it");
	info = mallinfo2();
	if (info.hblkhd < size) {
		print_mallinfo2("Test malloc 2G", &info);
		tst_res(TFAIL, "hblkhd member of struct mallinfo2 overflow?");
	} else {
		tst_res(TPASS, "hblkhd member of struct mallinfo2 doesn't overflow");
	}
	free(buf);
}

void main(){
	setup();
	cleanup();
}
