#include "incl.h"

pid_t real_pid, fake_pid, int_min_pid;

struct tcase {
	int test_sig;
	int exp_errno;
	pid_t *pid;
} tcases[] = {
	{2000, EINVAL, &real_pid},
	{SIGKILL, ESRCH, &fake_pid},
	{SIGKILL, ESRCH, &int_min_pid}
};

int  verify_kill(unsigned int n)
{
	struct tcase *tc = &tcases[n];
kill(*tc->pid, tc->test_sig);
	if (TST_RET != -1) {
		tst_res(TFAIL, "kill should fail but not, return %ld", TST_RET);
		return;
	}
	if (tc->exp_errno == TST_ERR)
		tst_res(TPASS | TTERRNO, "kill failed as expected");
	else
		tst_res(TFAIL | TTERRNO, "kill expected %s but got",
			tst_strerrno(tc->exp_errno));
}

void setup(void)
{
	real_pid = getpid();
	fake_pid = tst_get_unused_pid();
	int_min_pid = INT_MIN;
}

void main(){
	setup();
	cleanup();
}
