#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/limits.h>
#include <bits/stdint-uintn.h>
#include <pwd.h>

struct tst_device {
	const char *dev;
	const char *fs_type;
	uint64_t size;
};

extern struct tst_device *tst_device;