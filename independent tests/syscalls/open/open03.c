#include "incl.h"
#define TEST_FILE "testfile"

int  verify_open(
{
	TST_EXP_FD(open(TEST_FILE, O_RDWR | O_CREAT, 0700));
	close(TST_RET);
	unlink(TEST_FILE);
}

void main(){
	setup();
	cleanup();
}
