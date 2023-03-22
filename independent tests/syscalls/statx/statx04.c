#include "incl.h"
#define _GNU_SOURCE
#define MOUNT_POINT "mntpoint"
#define TESTDIR MOUNT_POINT "/testdir"
#define ATTR(x) {.attr = x, .name = #x}

struct {
	uint64_t attr;
	const char *name;
} attr_list[] = {
	ATTR(STATX_ATTR_COMPRESSED),
	ATTR(STATX_ATTR_APPEND),
	ATTR(STATX_ATTR_IMMUTABLE),
	ATTR(STATX_ATTR_NODUMP)
};

uint64_t expected_mask;

void setup(void)
{
	size_t i;
	int fd;
	mkdir(TESTDIR, 0777);
	fd = open(TESTDIR, O_RDONLY | O_DIRECTORY);
ioctl(fd, FS_IOC_GETFLAGS, &i);
	close(fd);
	if (TST_RET == -1 && TST_ERR == ENOTTY)
		tst_brk(TCONF | TTERRNO, "Inode attributes not supported");
	if (TST_RET)
		tst_brk(TBROK | TTERRNO, "Unexpected ioctl() error");
	for (i = 0, expected_mask = 0; i < ARRAY_SIZE(attr_list); i++)
		expected_mask |= attr_list[i].attr;
	if (!strcmp(tst_device->fs_type, "xfs") || !strcmp(tst_device->fs_type, "tmpfs"))
		expected_mask &= ~STATX_ATTR_COMPRESSED;
	if (!strcmp(tst_device->fs_type, "btrfs") && tst_kvercmp(4, 13, 0) < 0)
		tst_brk(TCONF, "statx() attributes not supported on Btrfs");
}

void run(void)
{
	size_t i;
	struct statx buf;
	TST_EXP_PASS_SILENT(statx(AT_FDCWD, TESTDIR, 0, 0, &buf));
	for (i = 0; i < ARRAY_SIZE(attr_list); i++) {
		if (!(expected_mask & attr_list[i].attr))
			continue;
		if (buf.stx_attributes_mask & attr_list[i].attr)
			tst_res(TPASS, "%s is supported", attr_list[i].name);
		else
			tst_res(TFAIL, "%s not supported", attr_list[i].name);
	}
}

void main(){
	setup();
	cleanup();
}
