#include "incl.h"
#define MODULE_NAME	"init_module.ko"

unsigned long size, zero_size;

int kernel_lockdown;

void *buf, *faulty_buf, *null_buf;

struct tst_cap cap_req = TST_CAP(TST_CAP_REQ, CAP_SYS_MODULE);

struct tst_cap cap_drop = TST_CAP(TST_CAP_DROP, CAP_SYS_MODULE);

struct tcase {
	const char *name;
	void **buf;
	unsigned long *size;
	const char *param;
	int cap;
	int skip_in_lockdown;
	int exp_errno;
} tcases[] = {
	{"NULL-buffer", &null_buf, &size, "", 0, 0, EFAULT},
	{"faulty-buffer", &faulty_buf, &size, "", 0, 0, EFAULT},
	{"null-param", &buf, &size, NULL, 0, 1, EFAULT},
	{"zero-size", &buf, &zero_size, "", 0, 0, ENOEXEC},
	{"invalid_param", &buf, &size, "status=invalid", 0, 1, EINVAL},
	{"no-perm", &buf, &size, "", 1, 0, EPERM},
	{"module-exists", &buf, &size, "", 0, 1, EEXIST},
};

void setup(void)
{
	struct stat sb;
	int fd;
	tst_module_exists(MODULE_NAME, NULL);
	kernel_lockdown = tst_lockdown_enabled();
	fd = open(MODULE_NAME, O_RDONLY|O_CLOEXEC);
	fstat(fd, &sb);
	size = sb.st_size;
	buf = mmap(0, size, PROT_READ|PROT_EXEC, MAP_PRIVATE, fd, 0);
	close(fd);
	faulty_buf = tst_get_bad_addr(NULL);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (tc->skip_in_lockdown && kernel_lockdown) {
		tst_res(TCONF, "Kernel is locked down, skipping %s", tc->name);
		return;
	}
	if (tc->cap)
		tst_cap_action(&cap_drop);
	if (tc->exp_errno == EEXIST)
		tst_module_load(MODULE_NAME, NULL);
	TST_EXP_FAIL(init_module(*tc->buf, *tc->size, tc->param),
		tc->exp_errno, "TestName: %s", tc->name);
	if (tc->exp_errno == EEXIST)
		tst_module_unload(MODULE_NAME);
	if (!TST_PASS && !TST_RET)
		tst_module_unload(MODULE_NAME);
	if (tc->cap)
		tst_cap_action(&cap_req);
}

void cleanup(void)
{
	munmap(buf, size);
}

void main(){
	setup();
	cleanup();
}
