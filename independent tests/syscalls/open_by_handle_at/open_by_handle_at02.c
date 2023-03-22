#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "test_file"
#define FOO_SYMLINK "foo_symlink"

struct file_handle high_fh = {.handle_bytes = MAX_HANDLE_SZ + 1}, *high_fhp = &high_fh;

struct file_handle zero_fh, *zero_fhp = &zero_fh;

struct file_handle *valid_fhp, *invalid_fhp, *link_fhp;

struct tst_cap cap_req = TST_CAP(TST_CAP_REQ, CAP_DAC_READ_SEARCH);

struct tst_cap cap_drop = TST_CAP(TST_CAP_DROP, CAP_DAC_READ_SEARCH);

struct tcase {
	const char *name;
	int dfd;
	struct file_handle **fhp;
	int flags;
	int cap;
	int exp_errno;
} tcases[] = {
	{"invalid-dfd", -1, &valid_fhp, O_RDWR, 0, EBADF},
	{"stale-dfd", 0, &valid_fhp, O_RDWR, 0, ESTALE},
	{"invalid-file-handle", AT_FDCWD, &invalid_fhp, O_RDWR, 0, EFAULT},
	{"high-file-handle-size", AT_FDCWD, &high_fhp, O_RDWR, 0, EINVAL},
	{"zero-file-handle-size", AT_FDCWD, &zero_fhp, O_RDWR, 0, EINVAL},
	{"no-capability", AT_FDCWD, &valid_fhp, O_RDWR, 1, EPERM},
	{"symlink", AT_FDCWD, &link_fhp, O_RDWR, 0, ELOOP},
};

void setup(void)
{
	void *faulty_address;
	int mount_id;
	touch(TEST_FILE, 0600, NULL);
	symlink(TEST_FILE, FOO_SYMLINK);
	faulty_address = tst_get_bad_addr(NULL);
	invalid_fhp = faulty_address;
	valid_fhp = allocate_file_handle(AT_FDCWD, TEST_FILE);
	if (!valid_fhp)
		return;
name_to_handle_at(AT_FDCWD, TEST_FILE, valid_fhp, &mount_id, 0);
	if (TST_RET)
		tst_res(TFAIL | TTERRNO, "name_to_handle_at() failed");
	link_fhp = tst_alloc(sizeof(*link_fhp) + valid_fhp->handle_bytes);
	link_fhp->handle_type = valid_fhp->handle_type;
	link_fhp->handle_bytes = valid_fhp->handle_bytes;
name_to_handle_at(AT_FDCWD, FOO_SYMLINK, link_fhp, &mount_id, 0);
	if (TST_RET)
		tst_res(TFAIL | TTERRNO, "name_to_handle_at() failed");
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int fd;
	if (tc->cap)
		tst_cap_action(&cap_drop);
fd = open_by_handle_at(tc->dfd, *tc->fhp, tc->flags);
	if (tc->cap)
		tst_cap_action(&cap_req);
	if (TST_RET != -1) {
		close(fd);
		tst_res(TFAIL, "%s: open_by_handle_at() passed unexpectedly",
			tc->name);
		return;
	}
	if (tc->exp_errno != TST_ERR) {
		tst_res(TFAIL | TTERRNO,
			"%s: open_by_handle_at() should fail with %s", tc->name,
			tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "%s: open_by_handle_at() failed as expected",
		tc->name);
}

void main(){
	setup();
	cleanup();
}
