#include "incl.h"
void setup();
int setup_test(int);
void cleanup_test(int);
void cleanup();
char *TCID = "mlockall02";
int TST_TOTAL = 3;
struct test_case_t {
} TC[] = {
	{
	MCL_CURRENT, ENOMEM, "Process exceeds max locked pages"}, {
	MCL_CURRENT, EPERM, "Not a superuser"}, {
	0, EINVAL, "Unknown flag"}
};
#if !defined(UCLINUX)
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
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
						 "mlock() Failed with wrong "
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
				if (i <= 1)
					tst_resm(TCONF,
						 "mlockall02 did not BEHAVE as expected.");
				else
					tst_brkm(TFAIL, cleanup,
						 "mlock() Failed, expected "
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
int setup_test(int i)
{
	struct rlimit rl;
	char nobody_uid[] = "nobody";
	struct passwd *ltpuser;
	switch (i) {
	case 0:
		rl.rlim_max = 10;
		rl.rlim_cur = 7;
		if (setrlimit(RLIMIT_MEMLOCK, &rl) != 0) {
			tst_resm(TWARN, "setrlimit failed to set the "
				 "resource for RLIMIT_MEMLOCK to check "
				 "for mlockall error %s\n", TC[i].edesc);
			return 1;
		}
		ltpuser = getpwnam(nobody_uid);
		if (seteuid(ltpuser->pw_uid) == -1) {
			tst_brkm(TBROK, cleanup, "seteuid() "
				"failed to change euid to %d "
				"errno = %d : %s",
				ltpuser->pw_uid, TEST_ERRNO,
				strerror(TEST_ERRNO));
				return 1;
		}
		return 0;
	case 1:
		rl.rlim_max = 0;
		rl.rlim_cur = 0;
		if (setrlimit(RLIMIT_MEMLOCK, &rl) != 0) {
			tst_resm(TWARN, "setrlimit failed to "
				"set the resource for "
				"RLIMIT_MEMLOCK to check for "
				"mlockall error %s\n", TC[i].edesc);
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
		seteuid(0);
		rl.rlim_max = -1;
		rl.rlim_cur = -1;
		if (setrlimit(RLIMIT_MEMLOCK, &rl) != 0) {
			tst_brkm(TFAIL, cleanup,
				 "setrlimit failed to reset the "
				 "resource for RLIMIT_MEMLOCK while "
				 "checking for mlockall error %s\n",
				 TC[i].edesc);
		}
		return;
	case 1:
		seteuid(cleanup, 0);
		return;
	}
}
void cleanup(void)
{
	return;
}
#else
int main(void)
{
	tst_resm(TINFO, "test is not available on uClinux");
	tst_exit();
}

