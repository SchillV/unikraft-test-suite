#include "incl.h"

int fd;

const char *TEST_DIR = "alpha";
#define MODES	S_IRWXU

int  verify_fchdir(
{
	TST_EXP_PASS(fchdir(fd));
}

void setup(void)
{
	mkdir(TEST_DIR, MODES);
	fd = open(TEST_DIR, O_RDONLY);
}

void cleanup(void)
{
	close(fd);
}

void main(){
	setup();
	cleanup();
}
