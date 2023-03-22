#include "incl.h"
#define _GNU_SOURCE

void reset_flags(void);

void check_flags(void);

void set_flag(const char *name);

int fd;
enum entry_type {
	ENTRY_DIR,
	ENTRY_FILE,
	ENTRY_SYMLINK,
};
struct testcase {
	const char *name;
	enum entry_type type;
	int create:1;
	int found:1;
};
struct testcase testcases[] = {
	{.name = ".",       .create = 0, .type = ENTRY_DIR},
	{.name = "..",      .create = 0, .type = ENTRY_DIR},
	{.name = "dir",     .create = 1, .type = ENTRY_DIR},
	{.name = "file",    .create = 1, .type = ENTRY_FILE},
	{.name = "symlink", .create = 1, .type = ENTRY_SYMLINK},
};
#define BUFSIZE 512

void *dirp;

void run(void)
{
	int rval;
	fd = open(".", O_RDONLY|O_DIRECTORY);
	rval = tst_getdents(fd, dirp, BUFSIZE);
	if (rval < 0) {
		if (errno == ENOSYS)
			tst_brk(TCONF, "syscall not implemented");
		else {
			tst_res(TFAIL | TERRNO, "getdents failed unexpectedly");
			return;
		}
	}
	if (rval == 0) {
		tst_res(TFAIL, "getdents failed - returned end of directory");
		return;
	}
	reset_flags();
	void *recp = dirp;
	do {
		size_t d_reclen = tst_dirp_reclen(recp);
		const char *d_name = tst_dirp_name(recp);
		set_flag(d_name);
		tst_res(TINFO, "Found '%s'", d_name);
		rval -= d_reclen;
		recp += d_reclen;
	} while (rval > 0);
	check_flags();
	close(fd);
}

void reset_flags(void)
{
	size_t i;
	for (i = 0; i < ARRAY_SIZE(testcases); i++)
		testcases[i].found = 0;
}

void check_flags(void)
{
	size_t i;
	int err = 0;
	for (i = 0; i < ARRAY_SIZE(testcases); i++) {
		if (!testcases[i].found) {
			tst_res(TINFO, "Entry '%s' not found", testcases[i].name);
			err++;
		}
	}
	if (err)
		tst_res(TFAIL, "Some entries not found");
	else
		tst_res(TPASS, "All entries found");
}

void set_flag(const char *name)
{
	size_t i;
	for (i = 0; i < ARRAY_SIZE(testcases); i++) {
		if (!strcmp(name, testcases[i].name)) {
			if (testcases[i].found)
				tst_res(TFAIL, "Duplicate entry %s", name);
			testcases[i].found = 1;
			return;
		}
	}
	tst_res(TFAIL, "Unexpected entry '%s' found", name);
}

void setup(void)
{
	size_t i;
	getdents_info();
	if (!tst_variant) {
		for (i = 0; i < ARRAY_SIZE(testcases); i++) {
			if (!testcases[i].create)
				continue;
			switch (testcases[i].type) {
			case ENTRY_DIR:
				mkdir(testcases[i].name, 0777);
			break;
			case ENTRY_FILE:
				fprintf(testcases[i].name, " ");
			break;
			case ENTRY_SYMLINK:
				symlink("nonexistent", testcases[i].name);
			break;
			}
		}
	}
}

void main(){
	setup();
	cleanup();
}
