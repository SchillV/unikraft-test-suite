#include "incl.h"
#define MNTPOINT	"mntpoint"
#define MODE_RWX	07770
#define DIR_NAME	MNTPOINT "/X.%d"
#define DIR_NAME_GROUP	MNTPOINT "/X.%d.%d"
#define NCHILD		3

int child_groups = 2;

int test_time = 1;

int nfiles = 10;

volatile int done;

void test1(int child_num)
{
	int j;
	char tmpdir[MAXPATHLEN];
	while (!done) {
		for (j = 0; j < nfiles; j += NCHILD) {
			sprintf(tmpdir, DIR_NAME, j);
			TST_EXP_FAIL_SILENT(mkdir(tmpdir, MODE_RWX), EEXIST);
			if (!TST_PASS)
				break;
		}
	}
	tst_res(TPASS, "[%d] create dirs that already exist", child_num);
}

void test2(int child_num)
{
	int j;
	char tmpdir[MAXPATHLEN];
	while (!done) {
		for (j = 1; j < nfiles; j += NCHILD) {
			sprintf(tmpdir, DIR_NAME, j);
			TST_EXP_FAIL_SILENT(rmdir(tmpdir), ENOENT);
			if (!TST_PASS)
				break;
		}
	}
	tst_res(TPASS, "[%d] remove dirs that do not exist", child_num);
}

void test3(int child_num)
{
	int j;
	char tmpdir[MAXPATHLEN];
	while (!done) {
		for (j = 2; j < nfiles; j += NCHILD) {
			sprintf(tmpdir, DIR_NAME_GROUP, j, child_num / NCHILD);
			TST_EXP_PASS_SILENT(mkdir(tmpdir, MODE_RWX));
			if (!TST_PASS)
				break;
		}
		for (j = 2; j < nfiles; j += NCHILD) {
			sprintf(tmpdir, DIR_NAME_GROUP, j, child_num / NCHILD);
			TST_EXP_PASS_SILENT(rmdir(tmpdir));
			if (!TST_PASS)
				break;
		}
	}
	tst_res(TPASS, "[%d] create/remove dirs", child_num);
}

void *child_thread_func(void *arg)
{
	void (*tests[NCHILD])(int) = { test1, test2, test3 };
	int child_num = (long)arg;
	tests[child_num % NCHILD](child_num);
	done = 1;
	return NULL;
}

int  verify_mkdir(
{
	pthread_t child_thread[NCHILD * child_groups];
	long i;
	done = 0;
	for (i = 0; i < child_groups * NCHILD; i++) {
		pthread_create(&child_thread[i], NULL,
			child_thread_func, (void *)i);
	}
	sleep(test_time);
	done = 1;
	for (i = 0; i < child_groups * NCHILD; i++)
		pthread_join(child_thread[i], NULL);
}

void setup(void)
{
	int j;
	char tmpdir[MAXPATHLEN];
	for (j = 0; j < nfiles; j += NCHILD) {
		sprintf(tmpdir, DIR_NAME, j);
		mkdir(tmpdir, MODE_RWX);
	}
}

void main(){
	setup();
	cleanup();
}
