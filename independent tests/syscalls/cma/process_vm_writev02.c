#include "incl.h"

uintptr_t *data_ptr;

char *str_buffsize;

int bufsize = 100000;

int  child_alloc_and_verify(int buffsize)
{
	char foo[buffsize];
	int i;
	int err;
	tst_res(TINFO, "child 0: allocate memory");
	memset(foo, 'a', buffsize);
	TST_CHECKPOINT_WAKE_AND_WAIT(0);
	err = 0;
	for (i = 0; i < buffsize; i++)
		if (foo[i] != 'w')
			err++;
	if (err)
		tst_res(TFAIL, "child 0: found %d differences from expected data", err);
	else
		tst_res(TPASS, "child 0: read back expected data");
}

void child_write(int buffsize, pid_t pid_alloc)
{
	char lp[buffsize];
	struct iovec local, remote;
	tst_res(TINFO, "child 1: write to the same memory location");
	memset(lp, 'w', buffsize);
	local.iov_base = lp;
	local.iov_len = buffsize;
	remote.iov_base = (void *)*data_ptr;
	remote.iov_len = buffsize;
	TST_EXP_POSITIVE(tst_syscall(__NR_process_vm_writev, pid_alloc, &local,
				     1UL, &remote, 1UL, 0UL));
	if (TST_RET != buffsize) {
		tst_brk(TBROK, "process_vm_writev: expected %d bytes but got %ld",
			buffsize, TST_RET);
	}
}

void setup(void)
{
	tst_syscall(__NR_process_vm_writev, getpid(), NULL, 0UL, NULL, 0UL, 0UL);
	if (tst_parse_int(str_buffsize, &bufsize, 1, INT_MAX))
		tst_brk(TBROK, "Invalid buffer size '%s'", str_buffsize);
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
	pid_t pid_alloc;
	pid_t pid_write;
	int status;
	pid_alloc = fork();
	if (!pid_alloc) {
int 		child_alloc_and_verify(bufsize);
		return;
	}
	TST_CHECKPOINT_WAIT(0);
	pid_write = fork();
	if (!pid_write) {
		child_write(bufsize, pid_alloc);
		return;
	}
	waitpid(pid_write, &status, 0);
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		tst_res(TFAIL, "write child: %s", tst_strstatus(status));
	TST_CHECKPOINT_WAKE(0);
}

void main(){
	setup();
	cleanup();
}
