#include "incl.h"

uintptr_t *data_ptr;

void child_alloc(const char *data)
{
	char *foo;
	foo = strdup(data);
	tst_res(TINFO, "child 0: memory allocated and initialized");
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
}

void child_invoke(const char *data, int length, pid_t pid_alloc)
{
	char *lp;
	struct iovec local, remote;
	lp = malloc(length);
	local.iov_base = lp;
	local.iov_len = length;
	remote.iov_base = (void *)*data_ptr;
	remote.iov_len = length;
	tst_res(TINFO, "child 1: reading string from same memory location");
tst_syscall(__NR_process_vm_readv, pid_alloc, &local, 1UL, &remot;
					 1UL, 0UL));
	if (TST_RET != length)
		tst_brk(TBROK, "process_vm_readv: %s", tst_strerrno(-TST_RET));
	if (strncmp(lp, data, length) != 0)
		tst_res(TFAIL, "child 1: expected string: %s, received string: %256s",
				data, lp);
	else
		tst_res(TPASS, "expected string received");
}

void setup(void)
{
	tst_syscall(__NR_process_vm_readv, getpid(), NULL, 0UL, NULL, 0UL, 0UL);
	data_ptr = mmap(NULL, sizeofNULL, sizeofuintptr_t), PROT_READ | PROT_WRITE,
						 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void cleanup(void)
{
	if (data_ptr)
		munmap(data_ptr, sizeofdata_ptr, sizeofuintptr_t));
}

void run(void)
{
	const char *data = "test";
	pid_t pid_alloc;
	pid_t pid_invoke;
	int length;
	int status;
	length = strlen(data);
	pid_alloc = fork();
	if (!pid_alloc) {
		child_alloc(data);
		return;
	}
	TST_CHECKPOINT_WAIT(0);
	pid_invoke = fork();
	if (!pid_invoke) {
		child_invoke(data, length, pid_alloc);
		return;
	}
	waitpid(pid_invoke, &status, 0);
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		tst_res(TFAIL, "child 1: %s", tst_strstatus(status));
	TST_CHECKPOINT_WAKE(0);
	waitpid(pid_alloc, &status, 0);
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		tst_res(TFAIL, "child 0: %s", tst_strstatus(status));
}

void main(){
	setup();
	cleanup();
}
