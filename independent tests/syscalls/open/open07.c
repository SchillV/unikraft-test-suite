#include "incl.h"

void setup(void);

void cleanup(void);

void setupfunc_test1();

void setupfunc_test2();

void setupfunc_test3();

void setupfunc_test4();

void setupfunc_test5();
char *TCID = "open07";
int TST_TOTAL = 5;

int fd1, fd2;

struct test_case_t {
	char *desc;
	char filename[100];
	int flags;
	int mode;
	void (*setupfunc) ();
	int exp_errno;
} TC[] = {
	{"Test for ELOOP on f2: f1 -> f2", {},
	 O_NOFOLLOW, 00700, setupfunc_test1, ELOOP},
	{"Test for ELOOP on d2: d1 -> d2", {},
	 O_NOFOLLOW, 00700, setupfunc_test2, ELOOP},
	{"Test for ELOOP on f3: f1 -> f2 -> f3", {},
	 O_NOFOLLOW, 00700, setupfunc_test3, ELOOP},
	{"Test for ELOOP on d3: d1 -> d2 -> d3", {},
	 O_NOFOLLOW, 00700, setupfunc_test4, ELOOP},
	{"Test for success on d2: d1 -> d2", {},
	 O_NOFOLLOW, 00700, setupfunc_test5, 0},
	{NULL, {}, 0, 0, NULL, 0}
};
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (i = 0; i < TST_TOTAL; i++) {
		if (TC[i].setupfunc != NULL)
			TC[i].setupfunc();
	}
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; TC[i].desc != NULL; ++i) {
open(TC[i].filename, TC[i].flags, TC[i].mode);
			if (TC[i].exp_errno != 0) {
				if (TEST_RETURN != -1) {
					tst_resm(TFAIL, "open succeeded "
						 "unexpectedly");
				}
				if (TEST_ERRNO != TC[i].exp_errno) {
					tst_resm(TFAIL, "open returned "
						 "unexpected errno, expected: "
						 "%d, got: %d",
						 TC[i].exp_errno, TEST_ERRNO);
				} else {
					tst_resm(TPASS, "open returned "
						 "expected ELOOP error");
				}
			} else {
				if (TEST_RETURN == -1) {
					tst_resm(TFAIL, "open failed "
						 "unexpectedly with errno %d",
						 TEST_ERRNO);
				} else {
					tst_resm(TPASS, "open succeeded as "
						 "expected");
				}
			}
			if (TEST_RETURN != -1)
				close(TEST_RETURN);
		}
	}
	cleanup();
	tst_exit();
}

void setupfunc_test1(void)
{
	char file1[100], file2[100];
	sprintf(file1, "open03.1.%d", getpid());
	sprintf(file2, "open03.2.%d", getpid());
	fd1 = creat(cleanup, file1, 00700);
	symlink(cleanup, file1, file2);
	strcpy(TC[0].filename, file2);
}

void setupfunc_test2(void)
{
	char file1[100], file2[100];
	sprintf(file1, "open03.3.%d", getpid());
	sprintf(file2, "open03.4.%d", getpid());
	mkdir(cleanup, file1, 00700);
	symlink(cleanup, file1, file2);
	strcpy(TC[1].filename, file2);
}

void setupfunc_test3(void)
{
	char file1[100], file2[100], file3[100];
	sprintf(file1, "open03.5.%d", getpid());
	sprintf(file2, "open03.6.%d", getpid());
	sprintf(file3, "open03.7.%d", getpid());
	fd2 = creat(cleanup, file1, 00700);
	symlink(cleanup, file1, file2);
	symlink(cleanup, file2, file3);
	strcpy(TC[2].filename, file3);
}

void setupfunc_test4(void)
{
	char file1[100], file2[100], file3[100];
	sprintf(file1, "open03.8.%d", getpid());
	sprintf(file2, "open03.9.%d", getpid());
	sprintf(file3, "open03.10.%d", getpid());
	mkdir(cleanup, file1, 00700);
	symlink(cleanup, file1, file2);
	symlink(cleanup, file2, file3);
	strcpy(TC[3].filename, file3);
}

void setupfunc_test5(void)
{
	char file1[100], file2[100];
	sprintf(file1, "open11.3.%d", getpid());
	sprintf(file2, "open12.4.%d", getpid());
	mkdir(cleanup, file1, 00700);
	symlink(cleanup, file1, file2);
	strcpy(TC[4].filename, file2);
	strcat(TC[4].filename, "/");
}

void setup(void)
{
	umask(0);
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
}

void cleanup(void)
{
	close(fd1);
	close(fd2);
	tst_rmdir();
}

