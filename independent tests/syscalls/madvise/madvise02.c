#include "incl.h"
#define MAP_SIZE (4 * 1024)
#define TEST_FILE "testfile"
#define STR "abcdefghijklmnopqrstuvwxyz12345\n"
#define KSM_SYS_DIR	"/sys/kernel/mm/ksm"

struct stat st;

long pagesize;

char *file1;

char *file2;

char *file3;

char *shared_anon;

char *ptr_addr;

char *tmp_addr;

char *nonalign;

struct tcase {
	int advice;
	char *name;
	char **addr;
	int exp_errno;
	int skip;
} tcases[] = {
	{MADV_NORMAL,      "MADV_NORMAL",      &nonalign, EINVAL, 0},
	{1212,             "MADV_NORMAL",      &file1,    EINVAL, 0},
	{MADV_REMOVE,      "MADV_REMOVE",      &file1,    EINVAL, 0},
	{MADV_DONTNEED,    "MADV_DONTNEED",    &file1,    EINVAL, 1},
	{MADV_MERGEABLE,   "MADV_MERGEABLE",   &file1,    EINVAL, 0},
	{MADV_UNMERGEABLE, "MADV_UNMERGEABLE", &file1,    EINVAL, 0},
	{MADV_NORMAL,      "MADV_NORMAL",      &file2,    ENOMEM, 0},
	{MADV_WILLNEED,    "MADV_WILLNEED",    &file2,    ENOMEM, 0},
	{MADV_WILLNEED,    "MADV_WILLNEED",    &tmp_addr,  EBADF, 0},
	{MADV_FREE,        "MADV_FREE",        &file1,    EINVAL, 0},
	{MADV_WIPEONFORK,  "MADV_WIPEONFORK",  &file1,    EINVAL, 0},
	{MADV_WIPEONFORK,  "MADV_WIPEONFORK shared_anon", &shared_anon, EINVAL, 0},
	{MADV_WIPEONFORK,  "MADV_WIPEONFORK private file backed", &file3, EINVAL, 0},
};

void tcases_filter(void)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(tcases); i++) {
		struct tcase *tc = &tcases[i];
		switch (tc->advice) {
		case MADV_DONTNEED:
#if !defined(UCLINUX)
			if (mlock(file1, st.st_size) < 0)
				tst_brk(TBROK | TERRNO, "mlock failed");
			tc->skip = 0;
		break;
		case MADV_MERGEABLE:
		case MADV_UNMERGEABLE:
			 * skip EINVAL test for MADV_MERGEABLE. */
			if (access(KSM_SYS_DIR, F_OK) == 0)
				tc->skip = 1;
		break;
		case MADV_WILLNEED:
			 * anon mem doesn't return -EBADF now, as now we support
			 * swap prefretch. */
			if (tc->exp_errno == EBADF)
				tc->skip = 1;
		break;
		case MADV_FREE:
			if ((tst_kvercmp(4, 5, 0)) < 0)
				tc->skip = 1;
		break;
		case MADV_WIPEONFORK:
			if ((tst_kvercmp(4, 14, 0)) < 0)
				tc->skip = 1;
		break;
		default:
		break;
		}
	}
}

void setup(void)
{
	int i, fd;
	fd = open(TEST_FILE, O_RDWR | O_CREAT, 0664);
	pagesize = getpagesize();
	for (i = 0; i < (pagesize / 2); i++)
		write(1, fd, STR, sizeof1, fd, STR, sizeofSTR) - 1);
	fstat(fd, &st);
	file1 = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	file2 = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	file3 = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	shared_anon = mmap(0, MAP_SIZE, PROT_READ, MAP_SHARED |
			MAP_ANONYMOUS, -1, 0);
	nonalign = file1 + 100;
	ptr_addr = malloc(st.st_size);
	tmp_addr = (void*)LTP_ALIGN((long)ptr_addr, pagesize);
	munmap(file2 + st.st_size - pagesize, pagesize);
	close(fd);
	tcases_filter();
}

void advice_test(unsigned int i)
{
	struct tcase *tc = &tcases[i];
	if (tc->skip == 1) {
		tst_res(TCONF, "%s is not supported", tc->name);
		return;
	}
madvise(*(tc->addr), st.st_size, tc->advice);
	if (TST_RET == -1) {
		if (TST_ERR == tc->exp_errno) {
			tst_res(TPASS | TTERRNO, "%s failed as expected", tc->name);
		} else {
			tst_res(TFAIL | TTERRNO,
					"%s failed unexpectedly; expected - %d : %s",
					tc->name, tc->exp_errno,
					tst_strerrno(TFAIL | TTERRNO));
		}
	} else {
		tst_res(TFAIL, "madvise succeeded unexpectedly");
	}
}

void cleanup(void)
{
	free(ptr_addr);
	munmap(file1, st.st_size);
	munmap(file2, st.st_size - pagesize);
	munmap(file3, st.st_size);
	munmap(shared_anon, MAP_SIZE);
}

void main(){
	setup();
	cleanup();
}
