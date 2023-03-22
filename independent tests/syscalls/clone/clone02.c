#include "incl.h"
#if defined UCLINUX && !__THROW
#define __THROW
#endif
#define _GNU_SOURCE
#define FLAG_ALL (CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|SIGCHLD)
#define FLAG_NONE SIGCHLD
#define PARENT_VALUE 1
#define CHILD_VALUE 2
#define TRUE 1
#define FALSE 0

void setup(void);

int test_setup(void);

void cleanup(void);

void test_cleanup(void);

int child_fn();

int parent_test1(void);

int parent_test2(void);

int test_VM(void);

int test_FS(void);

int test_FILES(void);

int test_SIG(void);

int modified_VM(void);

int modified_FS(void);

int modified_FILES(void);

int modified_SIG(void);

void sig_child_defined_handler(int);

void sig_default_handler();

int fd_parent;

char file_name[25];

int parent_variable;

char cwd_parent[FILENAME_MAX];

int parent_got_signal, child_pid;
char *TCID = "clone02";
struct test_case_t {
	int flags;
	int (*parent_fn) ();
} test_cases[] = {
	{
	FLAG_ALL, parent_test1}, {
	FLAG_NONE, parent_test2}
};
int TST_TOTAL = sizeof(test_cases) / sizeof(test_cases[0]);
int main(int ac, char **av)
{
	int lc;
	void *child_stack;
	int status, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	child_stack = malloc(CHILD_STACK_SIZE);
	if (child_stack == NULL)
		tst_brkm(TBROK, cleanup, "Cannot allocate stack for child");
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; ++i) {
			if (test_setup() != 0) {
				tst_resm(TWARN, "test_setup() failed, skipping this test case");
				continue;
			}
ltp_clone(test_cases[i].flags, child_fn, NUL;
				       CHILD_STACK_SIZE, child_stack));
			if (TEST_RETURN == -1) {
				tst_resm(TFAIL | TTERRNO, "clone() failed");
				test_cleanup();
				continue;
			}
			if ((wait(&status)) == -1) {
				tst_resm(TWARN | TERRNO,
					 "wait failed; skipping testcase");
				test_cleanup();
				continue;
			}
			if (WTERMSIG(status))
				tst_resm(TWARN, "child exitied with signal %d",
					 WTERMSIG(status));
			 * Check the return value from child function and
			 * parent function. If both functions returned
			 * successfully, test passed, else failed
			 */
			if (WIFEXITED(status) && WEXITSTATUS(status) == 0 &&
			    test_cases[i].parent_fn())
				tst_resm(TPASS, "Test Passed");
			else
				tst_resm(TFAIL, "Test Failed");
			test_cleanup();
		}
	}
	free(child_stack);
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	if ((sprintf(file_name, "parent_file_%ld", syscall(__NR_gettid))) <= 0)
		tst_brkm(TBROK | TERRNO, cleanup, "sprintf() failed");
}

void cleanup(void)
{
	if (unlink(file_name) == -1)
		tst_resm(TWARN | TERRNO, "unlink(%s) failed", file_name);
	tst_rmdir();
}

int test_setup(void)
{
	struct sigaction def_act;
	if (getcwd(cwd_parent, sizeof(cwd_parent)) == NULL) {
		tst_resm(TWARN | TERRNO, "getcwd() failed in test_setup()");
		return -1;
	}
	 * Set value for parent_variable in parent, which will be
	 * changed by child in test_VM(), for testing CLONE_VM flag
	 */
	parent_variable = PARENT_VALUE;
	 * Open file from parent, which will be closed by
	 * child in test_FILES(), used for testing CLONE_FILES flag
	 */
	fd_parent = open(file_name, O_CREAT | O_RDWR, 0777);
	if (fd_parent == -1) {
		tst_resm(TWARN | TERRNO, "open() failed in test_setup()");
		return -1;
	}
	 * set parent_got_signal to FALSE, used for testing
	 * CLONE_SIGHAND flag
	 */
	parent_got_signal = FALSE;
	def_act.sa_handler = sig_default_handler;
	def_act.sa_flags = SA_RESTART;
	sigemptyset(&def_act.sa_mask);
	if (sigaction(SIGUSR2, &def_act, NULL) == -1) {
		tst_resm(TWARN | TERRNO, "sigaction() failed in test_setup()");
		return -1;
	}
	return 0;
}

void test_cleanup(void)
{
	chdir(cleanup, cwd_parent);
}

int child_fn(void)
{
	child_pid = syscall(__NR_gettid);
	if (test_VM() == 0 && test_FILES() == 0 && test_FS() == 0 &&
	    test_SIG() == 0)
		_exit(0);
	_exit(1);
}

int parent_test1(void)
{
	 * For first test case (with all flags set), all resources are
	 * shared between parent and child. So whatever changes made by
	 * child should get reflected in parent also. modified_*()
	 * functions check this. All of them should return 1 for
	 * parent_test1() to return 1
	 */
	if (modified_VM() && modified_FILES() && modified_FS() &&
	    modified_SIG())
		return 0;
	return -1;
}

int parent_test2(void)
{
	 * For second test case (with no resouce shared), all of the
	 * modified_*() functions should return 0 for parent_test2()
	 * to return 1
	 */
	if (modified_VM() || modified_FILES() || modified_FS() ||
	    modified_SIG())
		return 0;
	return -1;
}

int test_VM(void)
{
	parent_variable = CHILD_VALUE;
	return 0;
}

int test_FILES(void)
{
	if (close(fd_parent) == -1) {
		tst_resm(TWARN | TERRNO, "close failed in test_FILES");
		return -1;
	}
	return 0;
}

int test_FS(void)
{
	char *test_tmpdir;
	int rval;
	test_tmpdir = tst_get_tmpdir();
	if (test_tmpdir == NULL) {
		tst_resm(TWARN | TERRNO, "tst_get_tmpdir failed");
		rval = -1;
	} else if (chdir(test_tmpdir) == -1) {
		tst_resm(TWARN | TERRNO, "chdir failed in test_FS");
		rval = -1;
	} else {
		rval = 0;
	}
	free(test_tmpdir);
	return rval;
}

int test_SIG(void)
{
	struct sigaction new_act;
	new_act.sa_handler = sig_child_defined_handler;
	new_act.sa_flags = SA_RESTART;
	sigemptyset(&new_act.sa_mask);
	if (sigaction(SIGUSR2, &new_act, NULL) == -1) {
		tst_resm(TWARN | TERRNO, "signal failed in test_SIG");
		return -1;
	}
	if (kill(getppid(), SIGUSR2) == -1) {
		tst_resm(TWARN | TERRNO, "kill failed in test_SIG");
		return -1;
	}
	return 0;
}

int modified_VM(void)
{
	if (parent_variable == CHILD_VALUE)
		return 1;
	return 0;
}

int modified_FILES(void)
{
	char buff[20];
	if (((read(fd_parent, buff, sizeof(buff))) == -1) && (errno == EBADF))
		return 1;
	if ((close(fd_parent)) == -1)
		tst_resm(TWARN | TERRNO, "close() failed in modified_FILES()");
	return 0;
}

int modified_FS(void)
{
	char cwd[FILENAME_MAX];
	if ((getcwd(cwd, sizeof(cwd))) == NULL)
		tst_resm(TWARN | TERRNO, "getcwd() failed");
	if (!(strcmp(cwd, cwd_parent)))
		return 0;
	return 1;
}

int modified_SIG(void)
{
	if (parent_got_signal)
		 * parent came through sig_child_defined_handler()
		 * this means child has changed parent's handler
		 */
		return 1;
	return 0;
}

void sig_child_defined_handler(int pid)
{
	if ((syscall(__NR_gettid)) == child_pid)
		tst_resm(TWARN, "Child got SIGUSR2 signal");
	else
		parent_got_signal = TRUE;
}

void sig_default_handler(void)
{
}

