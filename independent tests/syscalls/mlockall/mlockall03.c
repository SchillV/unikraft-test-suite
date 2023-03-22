#include "incl.h"
void setup();
int setup_test(int);
int compare(char s1[], char s2[]);
void cleanup_test(int);
void cleanup();
char *TCID = "mlockall03";
int TST_TOTAL = 3;
#if !defined(UCLINUX)
char *ref_release = "2.6.8\0";
struct test_case_t {
} TC[] = {
	{
	MCL_CURRENT, ENOMEM,
		    "tried to lock more memory than the limit permitted"}, {
	MCL_CURRENT, EPERM, "Not a superuser and RLIMIT_MEMLOCK was 0"}, {
	~(MCL_CURRENT | MCL_FUTURE), EINVAL, "Unknown flag"}
};
int main(int ac, char **av)
{
	int lc, i;
	struct utsname *buf;
	tst_parse_opts(ac, av, NULL, NULL);
	if ((buf = malloc((size_t)sizeof(struct utsname))) == NULL) {
		tst_brkm(TFAIL, NULL, "malloc failed for buf");
	}
	if (uname(buf) < 0) {
		tst_resm(TFAIL, "uname failed getting release number");
	}
	if ((compare(ref_release, buf->release)) <= 0) {
		tst_brkm(TCONF,
			 NULL,
			 "In Linux 2.6.8 and earlier this test will not run.");
	}
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			if (setup_test(i)) {
				tst_resm(TFAIL, "mlockall() Failed while setup "
					 "for checking error %s", TC[i].edesc);
				continue;
			}
mlockall(TC[i].flag);
			if (TEST_RETURN == -1) {
				if (TEST_ERRNO != TC[i].error)
					tst_brkm(TFAIL, cleanup,
						 "mlockall() Failed with wrong "
						 "errno, expected errno=%s, "
						 "got errno=%d : %s",
						 TC[i].edesc, TEST_ERRNO,
						 strerror(TEST_ERRNO));
				else
					tst_resm(TPASS,
						 "expected failure - errno "
						 "= %d : %s",
						 TEST_ERRNO,
						 strerror(TEST_ERRNO));
			} else {
				tst_brkm(TFAIL, cleanup,
					 "mlockall() Failed, expected "
					 "return value=-1, got %ld",
					 TEST_RETURN);
			}
			cleanup_test(i);
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_require_root();
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	return;
}
int compare(char s1[], char s2[])
{
	int i = 0;
	while (s1[i] == s2[i] && s1[i])
		i++;
	if (i < 4)
		return s2[i] - s1[i];
	if ((i == 4) && (isalnum(s2[i + 1]))) {
		return 1;
	} else {
		return s2[i] - s1[i];
	}
	return 0;
}
int setup_test(int i)
{
	struct rlimit rl;
	char nobody_uid[] = "nobody";
	struct passwd *ltpuser;
	switch (i) {
	case 0:
		ltpuser = getpwnam(nobody_uid);
		if (seteuid(ltpuser->pw_uid) == -1) {
			tst_brkm(TBROK, cleanup, "seteuid() failed to "
				 "change euid to %d errno = %d : %s",
				 ltpuser->pw_uid, TEST_ERRNO,
				 strerror(TEST_ERRNO));
			return 1;
		}
		rl.rlim_max = 10;
		rl.rlim_cur = 7;
		if (setrlimit(RLIMIT_MEMLOCK, &rl) != 0) {
			tst_resm(TWARN | TERRNO, "setrlimit failed to set the "
				 "resource for RLIMIT_MEMLOCK to check "
				 "for mlockall() error %s\n", TC[i].edesc);
			return 1;
		}
		return 0;
	case 1:
		rl.rlim_max = 0;
		rl.rlim_cur = 0;
		if (setrlimit(RLIMIT_MEMLOCK, &rl) != 0) {
			tst_resm(TWARN, "setrlimit failed to set the "
				 "resource for RLIMIT_MEMLOCK to check "
				 "for mlockall() error %s\n", TC[i].edesc);
			return 1;
		}
		ltpuser = getpwnam(nobody_uid);
		if (seteuid(ltpuser->pw_uid) == -1) {
			tst_brkm(TBROK, cleanup, "seteuid() failed to "
				 "change euid to %d errno = %d : %s",
				 ltpuser->pw_uid, TEST_ERRNO,
				 strerror(TEST_ERRNO));
			return 1;
		}
		return 0;
	}
	return 0;
}
void cleanup_test(int i)
{
	struct rlimit rl;
	switch (i) {
	case 0:
	case 1:
		seteuid(cleanup, 0);
		rl.rlim_max = -1;
		rl.rlim_cur = -1;
		if (setrlimit(RLIMIT_MEMLOCK, &rl) != 0) {
			tst_brkm(TFAIL, cleanup,
				 "setrlimit failed to reset the "
				 "resource for RLIMIT_MEMLOCK while "
				 "checking for mlockall() error %s\n",
				 TC[i].edesc);
		}
		return;
	}
}
#else
int main(void)
{
	tst_resm(TINFO, "test is not available on uClinux");
	tst_exit();
}
void cleanup(void)
{
	return;
}

