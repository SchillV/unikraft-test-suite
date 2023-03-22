#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "test_file"

struct file_handle fh, high_fh = {.handle_bytes = MAX_HANDLE_SZ + 1};

struct file_handle *valid_fhp = &fh, *invalid_fhp, *high_fhp = &high_fh;

int mount_id, *valid_mount_id = &mount_id, *invalid_mount_id;

const char *valid_path = TEST_FILE, *invalid_path, *empty_path = "";

struct tcase {
	const char *name;
	int dfd;
	const char **pathname;
	int flags;
	struct file_handle **fhp;
	int **mount_id;
	int exp_errno;
} tcases[] = {
	{"invalid-dfd", -1, &valid_path, 0, &valid_fhp, &valid_mount_id, EBADF},
	{"not a directory", 0, &valid_path, 0, &valid_fhp, &valid_mount_id, ENOTDIR},
	{"invalid-path", AT_FDCWD, &invalid_path, 0, &valid_fhp, &valid_mount_id, EFAULT},
	{"invalid-file-handle", AT_FDCWD, &valid_path, 0, &invalid_fhp, &valid_mount_id, EFAULT},
	{"zero-file-handle-size", AT_FDCWD, &valid_path, 0, &valid_fhp, &valid_mount_id, EOVERFLOW},
	{"high-file-handle-size", AT_FDCWD, &valid_path, 0, &high_fhp, &valid_mount_id, EINVAL},
	{"invalid-mount_id", AT_FDCWD, &valid_path, 0, &valid_fhp, &invalid_mount_id, EFAULT},
	{"invalid-flags", AT_FDCWD, &valid_path, -1, &valid_fhp, &valid_mount_id, EINVAL},
	{"empty-path", AT_FDCWD, &empty_path, 0, &valid_fhp, &valid_mount_id, ENOENT},
};

void setup(void)
{
	void *faulty_address;
	touch(TEST_FILE, 0600, NULL);
	faulty_address = tst_get_bad_addr(NULL);
	invalid_fhp = faulty_address;
	invalid_mount_id = faulty_address;
	invalid_path = faulty_address;
}

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	memset(&fh, 0, sizeof(fh));
name_to_handle_at(tc->dfd, *tc->pathname, *tc->fhp, *tc->mount_i;
			       tc->flags));
	if (TST_RET != -1) {
		tst_res(TFAIL, "%s: name_to_handle_at() passed unexpectedly",
			tc->name);
		return;
	}
	if (tc->exp_errno != TST_ERR) {
		tst_res(TFAIL | TTERRNO,
			"%s: name_to_handle_at() should fail with %s", tc->name,
			tst_strerrno(tc->exp_errno));
		return;
	}
	tst_res(TPASS | TTERRNO, "%s: name_to_handle_at() failed as expected",
		tc->name);
}

void main(){
	setup();
	cleanup();
}
