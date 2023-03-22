#include "incl.h"

pid_t pid;

struct __user_cap_header_struct *hdr;

struct __user_cap_data_struct *data;

struct tcase {
	int version;
	char *message;
} tcases[] = {
	{0x19980330, "LINUX_CAPABILITY_VERSION_1"},
	{0x20071026, "LINUX_CAPABILITY_VERSION_2"},
	{0x20080522, "LINUX_CAPABILITY_VERSION_3"},
};

int  verify_capget(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	hdr->version = tc->version;
	hdr->pid = pid;
	TST_EXP_PASS(tst_syscall(__NR_capget, hdr, data),
	             "capget() with %s", tc->message);
	if (data[0].effective & 1 << CAP_NET_RAW)
		tst_res(TFAIL, "capget() gets CAP_NET_RAW unexpectedly in pE");
	else
		tst_res(TPASS, "capget() doesn't get CAP_NET_RAW as expected in PE");
}

void setup(void)
{
	pid = getpid();
}

void main(){
	setup();
	cleanup();
}
