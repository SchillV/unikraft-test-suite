#include "incl.h"

pid_t unused_tid;

pid_t inval_tid = -1;
struct test_case_t {
	int *tid;
	int exp_errno;
} tc[] = {
	{&inval_tid, EINVAL},
	{&unused_tid, ESRCH}
};

void setup(void)
{
	unused_tid = tst_get_unused_pid();
}

void run(unsigned int i)
{
	TST_EXP_FAIL(tst_syscall(__NR_tkill, *(tc[i].tid), SIGUSR1),
		     tc[i].exp_errno, "tst_syscall(__NR_tkill) expecting %s",
			 tst_strerrno(tc[i].exp_errno));
}

void main(){
	setup();
	cleanup();
}
