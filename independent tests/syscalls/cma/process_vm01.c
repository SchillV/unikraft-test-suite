#include "incl.h"
struct process_vm_params {
	int len;
	char *ldummy;
	char *rdummy;
	pid_t pid;
	struct iovec *lvec;
	unsigned long liovcnt;
	struct iovec *rvec;
	unsigned long riovcnt;
	unsigned long flags;
};

char *str_read;

void (*test_params)(struct process_vm_params *params);

struct process_vm_params *alloc_params(void)
{
	struct process_vm_params *sane_params;
	int len;
	len = getpagesize();
	sane_params = malloc(sizeofsizeofstruct process_vm_params));
	sane_params->len = len;
	sane_params->ldummy = malloc(len);
	sane_params->rdummy = malloc(len);
	sane_params->lvec = malloc(sizeofsizeofstruct process_vm_params));
	sane_params->lvec->iov_base = sane_params->ldummy;
	sane_params->lvec->iov_len = len;
	sane_params->liovcnt = 1;
	sane_params->rvec = malloc(sizeofsizeofstruct process_vm_params));
	sane_params->rvec->iov_base = sane_params->rdummy;
	sane_params->rvec->iov_len = len;
	sane_params->riovcnt = 1;
	sane_params->flags = 0;
	sane_params->pid = getpid();
	return sane_params;
}

void free_params(struct process_vm_params *params)
{
	if (params) {
		free(params->ldummy);
		free(params->rdummy);
		free(params->lvec);
		free(params->rvec);
		free(params);
	}
}

void test_readv(struct process_vm_params *params)
{
tst_syscall(__NR_process_vm_read;
		params->pid,
		params->lvec, params->liovcnt,
		params->rvec, params->riovcnt,
		params->flags));
}

void test_writev(struct process_vm_params *params)
{
tst_syscall(__NR_process_vm_write;
		params->pid,
		params->lvec, params->liovcnt,
		params->rvec, params->riovcnt,
		params->flags));
}

void check_errno(long expected_errno)
{
	if (TST_ERR == expected_errno)
		tst_res(TPASS | TTERRNO, "expected failure");
	else if (TST_ERR == 0)
		tst_res(TFAIL, "call succeeded unexpectedly");
	else
		tst_res(TFAIL | TTERRNO, "unexpected failure - "
			"expected = %ld : %s, actual",
			expected_errno, strerror(expected_errno));
}

void test_sane_params(void)
{
	struct process_vm_params *sane_params;
	tst_res(TINFO, "Testing sane parameters");
	sane_params = alloc_params();
	test_params(sane_params);
	TST_EXP_EQ_LI(TST_RET, sane_params->len);
	free_params(sane_params);
}

void test_flags(void)
{
	struct process_vm_params *params;
	long flags[] = { -INT_MAX, -1, 1, INT_MAX, 0 };
	int flags_size = ARRAY_SIZE(flags) / sizeof(flags[0]);
	int i;
	params = alloc_params();
	for (i = 0; i < flags_size; i++) {
		params->flags = flags[i];
		tst_res(TINFO, "Testing flags=%ld", flags[i]);
		test_params(params);
		 * should fail with EINVAL
		 */
		if (flags[i] != 0) {
			TST_EXP_EQ_LI(TST_RET, -1);
			check_errno(EINVAL);
		} else {
			TST_EXP_EQ_LI(TST_RET, params->len);
		}
	}
	free_params(params);
}

void test_iov_len_overflow(void)
{
	struct process_vm_params *params;
	tst_res(TINFO, "Testing iov_len = -1");
	params = alloc_params();
	params->lvec->iov_len = -1;
	params->rvec->iov_len = -1;
	test_params(params);
	TST_EXP_EQ_LI(TST_RET, -1);
	check_errno(EINVAL);
	free_params(params);
}

void test_iov_invalid(void)
{
	struct process_vm_params *sane_params;
	struct process_vm_params params_copy;
	sane_params = alloc_params();
	tst_res(TINFO, "Testing lvec->iov_base = -1");
	params_copy = *sane_params;
	params_copy.lvec->iov_base = (void *)-1;
	test_params(&params_copy);
	TST_EXP_EQ_LI(TST_RET, -1);
	check_errno(EFAULT);
	tst_res(TINFO, "Testing rvec->iov_base = -1");
	params_copy = *sane_params;
	params_copy.rvec->iov_base = (void *)-1;
	test_params(&params_copy);
	TST_EXP_EQ_LI(TST_RET, -1);
	check_errno(EFAULT);
	tst_res(TINFO, "Testing lvec = -1");
	params_copy = *sane_params;
	params_copy.lvec = (void *)-1;
	test_params(&params_copy);
	TST_EXP_EQ_LI(TST_RET, -1);
	check_errno(EFAULT);
	tst_res(TINFO, "Testing rvec = -1");
	params_copy = *sane_params;
	params_copy.rvec = (void *)-1;
	test_params(&params_copy);
	TST_EXP_EQ_LI(TST_RET, -1);
	check_errno(EFAULT);
	free_params(sane_params);
}

void test_invalid_pid(void)
{
	pid_t invalid_pid = -1;
	struct process_vm_params *params;
	struct process_vm_params params_copy;
	params = alloc_params();
	tst_res(TINFO, "Testing invalid PID");
	params_copy = *params;
	params_copy.pid = invalid_pid;
	test_params(&params_copy);
	TST_EXP_EQ_LI(TST_RET, -1);
	check_errno(ESRCH);
	tst_res(TINFO, "Testing unused PID");
	params_copy = *params;
	invalid_pid = tst_get_unused_pid();
	params_copy.pid = invalid_pid;
	test_params(&params_copy);
	TST_EXP_EQ_LI(TST_RET, -1);
	check_errno(ESRCH);
	free_params(params);
}

void test_invalid_perm(void)
{
	char nobody_uid[] = "nobody";
	struct passwd *ltpuser;
	struct process_vm_params *params;
	pid_t child_pid;
	pid_t parent_pid;
	tst_res(TINFO, "Testing invalid permissions on given PID");
	parent_pid = getpid();
	child_pid = fork();
	if (!child_pid) {
		ltpuser = getpwnam(nobody_uid);
		setuid(ltpuser->pw_uid);
		params = alloc_params();
		params->pid = parent_pid;
		test_params(params);
		TST_EXP_EQ_LI(TST_RET, -1);
		check_errno(EPERM);
		free_params(params);
		return;
	}
	 * TFAIL/TPASS messages will arrive asynchronously
	 */
	tst_reap_children();
}

void test_invalid_protection(void)
{
	struct process_vm_params *sane_params;
	struct process_vm_params params_copy;
	void *data;
	int len;
	len = getpagesize();
	sane_params = alloc_params();
	data = mmap(NULL, len, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	tst_res(TINFO, "Testing data with invalid protection (lvec)");
	params_copy = *sane_params;
	params_copy.lvec->iov_base = data;
	test_params(&params_copy);
	TST_EXP_EQ_LI(TST_RET, -1);
	check_errno(EFAULT);
	tst_res(TINFO, "Testing data with invalid protection (rvec)");
	params_copy = *sane_params;
	params_copy.rvec->iov_base = data;
	test_params(&params_copy);
	TST_EXP_EQ_LI(TST_RET, -1);
	check_errno(EFAULT);
	munmap(data, len);
	free_params(sane_params);
}

void run(void)
{
	test_sane_params();
	test_flags();
	test_iov_len_overflow();
	test_iov_invalid();
	test_invalid_pid();
	test_invalid_perm();
	test_invalid_protection();
}

void setup(void)
{
	if (str_read) {
		tst_res(TINFO, "Selected process_vm_readv");
		test_params = test_readv;
	} else {
		tst_res(TINFO, "Selected process_vm_writev");
		test_params = test_writev;
	}
}

void main(){
	setup();
	cleanup();
}
