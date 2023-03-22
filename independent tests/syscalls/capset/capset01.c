#include "incl.h"

pid_t pid;

struct __user_cap_header_struct *header;

struct __user_cap_data_struct *data;

struct tcase {
	int version;
	char *message;
} tcases[] = {
	{0x19980330, "LINUX_CAPABILITY_VERSION_1"},
	{0x20071026, "LINUX_CAPABILITY_VERSION_2"},
	{0x20080522, "LINUX_CAPABILITY_VERSION_3"},
};

int  verify_capset(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	header->version = tc->version;
	header->pid = pid;
tst_syscall(__NR_capget, header, data);
	if (TST_RET == -1)
	      tst_brk(TFAIL | TTERRNO, "capget() failed");
	TST_EXP_PASS(tst_syscall(__NR_capset, header, data),
	             "capset() with %s", tc->message);
}

void setup(void)
{
	pid = getpid();
}

void main(){
	setup();
	cleanup();
}
