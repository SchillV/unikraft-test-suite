#include "incl.h"

void file_create(char *);

void fifo_create(char *);

struct test_case_t {
	void (*setupfunc)(char *);
	char *desc;
} tcases[] = {
	{file_create, "file"},
	{fifo_create, "fifo"},
};

void file_create(char *name)
{
	sprintf(name, "tfile_%d", getpid());
	touch(name, 0777, NULL);
}

void fifo_create(char *name)
{
	sprintf(name, "tfifo_%d", getpid());
	mkfifo(name, 0777);
}

int  verify_unlink(unsigned int n)
{
	char fname[255];
	struct test_case_t *tc = &tcases[n];
	tc->setupfunc(fname);
unlink(fname);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "unlink(%s) failed", tc->desc);
		return;
	}
	if (!access(fname, F_OK)) {
		tst_res(TFAIL, "unlink(%s) succeeded, but %s still existed",
			tc->desc, tc->desc);
		return;
	}
	tst_res(TPASS, "unlink(%s) succeeded", tc->desc);
}

void main(){
	setup();
	cleanup();
}
