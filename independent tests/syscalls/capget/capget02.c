#include "incl.h"

pid_t unused_pid;

struct __user_cap_header_struct *header;

struct __user_cap_data_struct *data, *bad_data;

struct tcase {
	int version;
	int pid;
	int exp_err;
	int flag;
	char *message;
} tcases[] = {
	{0x20080522, 0, EFAULT, 1, "bad address header"},
	{0x20080522, 0, EFAULT, 2, "bad address data"},
	{0, 0, EINVAL, 0, "bad version"},
	{0x20080522, -1, EINVAL, 0, "bad pid"},
	{0x20080522, 1, ESRCH, 0, "unused pid"},
};

int  verify_capget(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	header->version = tc->version;
	if (tc->pid == 1)
		header->pid = unused_pid;
	else
		header->pid = tc->pid;
	 * header must not be NULL. data may be NULL only when the user is
	 * trying to determine the preferred capability version format
	 * supported by the kernel. So use tst_get_bad_addr() to get
	 * this error.
	 */
	TST_EXP_FAIL(tst_syscall(__NR_capget, tc->flag - 1 ? header : NULL,
	                         tc->flag - 2 ? data : bad_data),
		     tc->exp_err, "capget() with %s", tc->message);
	 * When an unsupported version value is specified, it will
	 * return the kernel preferred value of _LINUX_CAPABILITY_VERSION_?.
	 * Since linux 2.6.26, version 3 is default. We use it.
	 */
	if (header->version != 0x20080522)
		tst_res(TFAIL, "kernel doesn't return preferred linux"
			" capability version when using bad version");
}

void setup(void)
{
	unused_pid = tst_get_unused_pid();
	bad_data = tst_get_bad_addr(NULL);
}

void main(){
	setup();
	cleanup();
}
