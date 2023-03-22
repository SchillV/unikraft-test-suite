#include "incl.h"
char Fname[255];
int fd;

void run(void)
{
dup(fd);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "dup failed");
	} else {
		tst_res(TPASS, "dup returned %ld",
			 TST_RET);
		close(TST_RET);
	}
}
void setup(void)
{
	fd = -1;
	sprintf(Fname, "dupfile");
	mkfifo(Fname, 0777);
	if ((fd = open(Fname, O_RDWR, 0700)) == -1)
		tst_brk(TBROK, "open failed");
}
void cleanup(void)
{
	if (fd != -1)
		if (close(fd) == -1)
			tst_res(TWARN | TERRNO, "close failed");
}

void main(){
	setup();
	cleanup();
}
