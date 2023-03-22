#include "incl.h"
#define MODULE_NAME	"finit_module.ko"
#define TEST_DIR	"test_dir"

char *mod_path;

int fd, fd_zero, fd_invalid = -1, fd_dir;

int kernel_lockdown;

struct tst_cap cap_req = TST_CAP(TST_CAP_REQ, CAP_SYS_MODULE);

struct tst_cap cap_drop = TST_CAP(TST_CAP_DROP, CAP_SYS_MODULE);
struct tcase {
	const char *name;
	int *fd;
	const char *param;
	int open_flags;
	int flags;
	int cap;
	int exp_errno;
	int skip_in_lockdown;
	void (*fix_errno)(struct tcase *tc);
};

void bad_fd_setup(struct tcase *tc)
{
	if (tst_kvercmp(4, 6, 0) < 0)
		tc->exp_errno = ENOEXEC;
	else
		tc->exp_errno = EBADF;
}

void dir_setup(struct tcase *tc)
{
	if (tst_kvercmp(4, 6, 0) < 0)
		tc->exp_errno = EISDIR;
	else
		tc->exp_errno = EINVAL;
}

struct tcase tcases[] = {
	{"invalid-fd", &fd_invalid, "", O_RDONLY | O_CLOEXEC, 0, 0, 0, 0,
		bad_fd_setup},
	{"zero-fd", &fd_zero, "", O_RDONLY | O_CLOEXEC, 0, 0, EINVAL, 0, NULL},
	{"null-param", &fd, NULL, O_RDONLY | O_CLOEXEC, 0, 0, EFAULT, 1, NULL},
	{"invalid-param", &fd, "status=invalid", O_RDONLY | O_CLOEXEC, 0, 0,
		EINVAL, 1, NULL},
	{"invalid-flags", &fd, "", O_RDONLY | O_CLOEXEC, -1, 0, EINVAL, 0,
		NULL},
	{"no-perm", &fd, "", O_RDONLY | O_CLOEXEC, 0, 1, EPERM, 0, NULL},
	{"module-exists", &fd, "", O_RDONLY | O_CLOEXEC, 0, 0, EEXIST, 1,
		NULL},
	{"file-not-readable", &fd, "", O_WRONLY | O_CLOEXEC, 0, 0, EBADF, 0,
		NULL},
	{"file-readwrite", &fd, "", O_RDWR | O_CLOEXEC, 0, 0, ETXTBSY, 0,
		NULL},
	{"directory", &fd_dir, "", O_RDONLY | O_CLOEXEC, 0, 0, 0, 0, dir_setup},
};

void setup(void)
{
	unsigned long int i;
	tst_module_exists(MODULE_NAME, &mod_path);
	kernel_lockdown = tst_lockdown_enabled();
	mkdir(TEST_DIR, 0700);
	fd_dir = open(TEST_DIR, O_DIRECTORY);
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		if (tcases[i].fix_errno)
			tcases[i].fix_errno(&tcases[i]);
	}
}

void cleanup(void)
{
	close(fd_dir);
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	if (tc->skip_in_lockdown && kernel_lockdown) {
		tst_res(TCONF, "Kernel is locked down, skipping %s", tc->name);
		return;
	}
	fd = open(mod_path, tc->open_flags);
	if (tc->cap)
		tst_cap_action(&cap_drop);
	if (tc->exp_errno == EEXIST)
		tst_module_load(MODULE_NAME, NULL);
	TST_EXP_FAIL(finit_module(*tc->fd, tc->param, tc->flags), tc->exp_errno,
		     "TestName: %s", tc->name);
	if (tc->exp_errno == EEXIST)
		tst_module_unload(MODULE_NAME);
	if (!TST_PASS && !TST_RET)
		tst_module_unload(MODULE_NAME);
	if (tc->cap)
		tst_cap_action(&cap_req);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
