#include "incl.h"
#define DEBUG 0
#define _GNU_SOURCE
#define SIGBAD 9999
void setup();
void cleanup();
char *TCID = "sigaction02";
int TST_TOTAL = 1;
volatile sig_atomic_t testcase_no;
void handler(int sig)
{
	if (DEBUG)
		tst_resm(TINFO, "Inside signal handler. Got signal: %d", sig);
	return;
}
int set_handler(int sig, int sig_to_mask, int flag)
{
	struct sigaction sa;
	int err;
	if (flag == 0) {
		sa.sa_sigaction = (void *)handler;
		sa.sa_flags = SA_NOMASK;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, sig_to_mask);
		err = sigaction(sig, &sa, NULL);
	} else if (flag == 1) {
		err = sigaction(sig, (void *)-1, NULL);
	} else if (flag == 2) {
		err = sigaction(sig, NULL, (void *)-1);
	} else
		err = -1;
	if (err == 0)
		return 0;
	else
		return errno;
}
int main(int ac, char **av)
{
	int ret;
	tst_parse_opts(ac, av, NULL, NULL);
	testcase_no = 1;
	if (DEBUG)
		tst_resm(TINFO, "Enter test %d: set handler for SIGKILL",
			 testcase_no);
	if ((ret = set_handler(SIGKILL, 0, 0)) == 0) {
		tst_resm(TFAIL, "sigaction() succeeded, should have failed");
	}
	if (ret != EINVAL) {
		tst_resm(TFAIL, "sigaction set incorrect errno. Expected "
			 "EINVAL, got: %d", ret);
	} else {
		tst_resm(TPASS, "call failed with expected EINVAL error");
	}
	testcase_no++;
	if (DEBUG)
		tst_resm(TINFO, "Enter test %d: set handler for SIGSTOP",
			 testcase_no);
	if ((ret = set_handler(SIGSTOP, 0, 0)) == 0) {
		tst_resm(TFAIL, "sigaction() succeeded, should have failed");
	}
	if (ret != EINVAL) {
		tst_resm(TFAIL, "sigaction set incorrect errno. Expected "
			 "EINVAL, got: %d", ret);
	} else {
		tst_resm(TPASS, "call failed with expected EINVAL error");
	}
	testcase_no++;
	if (DEBUG)
		tst_resm(TINFO, "Enter test %d: set handler for bad "
			 "signal number", testcase_no);
	if ((ret = set_handler(SIGBAD, 0, 0)) == 0) {
		tst_resm(TFAIL, "sigaction() succeeded, should have failed");
	}
	if (ret != EINVAL) {
		tst_resm(TFAIL, "sigaction set incorrect errno. Expected "
			 "EINVAL, got: %d", ret);
	} else {
		tst_resm(TPASS, "call failed with expected EINVAL error");
	}
#ifndef GLIBC_SIGACTION_BUG
	testcase_no++;
	if (DEBUG)
		tst_resm(TINFO, "Enter test %d: set handler with "
			 "bad \"act\" param", testcase_no);
	if ((ret = set_handler(SIGUSR1, 0, 1)) == 0) {
		tst_resm(TFAIL, "sigaction() succeeded, should have failed");
	}
	if (ret != EFAULT) {
		tst_resm(TFAIL, "sigaction set incorrect errno. Expected "
			 "EFAULT, got: %d", ret);
	} else {
		tst_resm(TPASS, "call failed with expected EFAULT error");
	}
	testcase_no++;
	if (DEBUG)
		tst_resm(TINFO, "Enter test %d: set handler with "
			 "bad \"oact\" param", testcase_no);
	if ((ret = set_handler(SIGUSR1, 0, 2)) == 0) {
		tst_resm(TFAIL, "sigaction() succeeded, should have failed");
	}
	if (ret != EFAULT) {
		tst_resm(TFAIL, "sigaction set incorrect errno. Expected "
			 "EFAULT, got: %d", ret);
	} else {
		tst_resm(TPASS, "call failed with expected EFAULT error");
	}
	tst_exit();
}

