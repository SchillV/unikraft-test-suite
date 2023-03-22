#include "incl.h"
#define _GNU_SOURCE
#define TEST_FILE "pkey_testfile"
#define STR "abcdefghijklmnopqrstuvwxyz12345\n"
#define PATH_VM_NRHPS "/proc/sys/vm/nr_hugepages"

int size;

struct tcase {
	unsigned long flags;
	unsigned long access_rights;
	char *name;
} tcases[] = {
	{0, PKEY_DISABLE_ACCESS, "PKEY_DISABLE_ACCESS"},
	{0, PKEY_DISABLE_WRITE, "PKEY_DISABLE_WRITE"},
};

void setup(void)
{
	int i, fd;
	check_pkey_support();
	if (tst_hugepages == test.hugepages.number)
		size = read_meminfo("Hugepagesize:") * 1024;
	else
		size = getpagesize();
	fd = open(TEST_FILE, O_RDWR | O_CREAT, 0664);
	for (i = 0; i < 128; i++)
		write(1, fd, STR, strlen1, fd, STR, strlenSTR));
	close(fd);
}

struct mmap_param {
	int prot;
	int flags;
	int fd;
} mmap_params[] = {
	{PROT_READ,  MAP_ANONYMOUS | MAP_PRIVATE, -1},
	{PROT_READ,  MAP_ANONYMOUS | MAP_SHARED, -1},
	{PROT_READ,  MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1},
	{PROT_READ,  MAP_ANONYMOUS | MAP_SHARED  | MAP_HUGETLB, -1},
	{PROT_READ,  MAP_PRIVATE, 0},
	{PROT_READ,  MAP_SHARED, 0},
	{PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1},
	{PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1},
	{PROT_WRITE, MAP_PRIVATE, 0},
	{PROT_WRITE, MAP_SHARED, 0},
	{PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1},
	{PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED  | MAP_HUGETLB, -1},
	{PROT_EXEC,  MAP_ANONYMOUS | MAP_PRIVATE, -1},
	{PROT_EXEC,  MAP_ANONYMOUS | MAP_SHARED, -1},
	{PROT_EXEC,  MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1},
	{PROT_EXEC,  MAP_ANONYMOUS | MAP_SHARED  | MAP_HUGETLB, -1},
	{PROT_EXEC,  MAP_PRIVATE, 0},
	{PROT_EXEC,  MAP_SHARED, 0},
	{PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1},
	{PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1},
	{PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1},
	{PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED  | MAP_HUGETLB, -1},
	{PROT_READ | PROT_WRITE, MAP_PRIVATE, 0},
	{PROT_READ | PROT_WRITE, MAP_SHARED, 0},
	{PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1},
	{PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_SHARED, -1},
	{PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1},
	{PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_SHARED  | MAP_HUGETLB, -1},
	{PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, 0},
	{PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, 0},
};

char *flag_to_str(int flags)
{
	switch (flags) {
	case MAP_PRIVATE:
		return "MAP_PRIVATE";
	case MAP_SHARED:
		return "MAP_SHARED";
	case MAP_ANONYMOUS | MAP_PRIVATE:
		return "MAP_ANONYMOUS|MAP_PRIVATE";
	case MAP_ANONYMOUS | MAP_SHARED:
		return "MAP_ANONYMOUS|MAP_SHARED";
	case MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB:
		return "MAP_ANONYMOUS|MAP_PRIVATE|MAP_HUGETLB";
	case MAP_ANONYMOUS | MAP_SHARED  | MAP_HUGETLB:
		return "MAP_ANONYMOUS|MAP_SHARED|MAP_HUGETLB";
	default:
		return "UNKNOWN FLAGS";
	}
}

void pkey_test(struct tcase *tc, struct mmap_param *mpa)
{
	pid_t pid;
	char *buffer;
	int pkey, status;
	int fd = mpa->fd;
	if (!tst_hugepages && (mpa->flags & MAP_HUGETLB)) {
		tst_res(TINFO, "Skip test on (%s) buffer", flag_to_str(mpa->flags));
		return;
	}
	if (fd == 0)
		fd = open(TEST_FILE, O_RDWR | O_CREAT, 0664);
	buffer = mmap(NULL, size, mpa->prot, mpa->flags, fd, 0);
	pkey = ltp_pkey_alloc(tc->flags, tc->access_rights);
	if (pkey == -1)
		tst_brk(TBROK | TERRNO, "pkey_alloc failed");
	tst_res(TINFO, "Set %s on (%s) buffer", tc->name, flag_to_str(mpa->flags));
	if (ltp_pkey_mprotect(buffer, size, mpa->prot, pkey) == -1)
		tst_brk(TBROK | TERRNO, "pkey_mprotect failed");
	pid = fork();
	if (pid == 0) {
		tst_no_corefile(0);
		switch (tc->access_rights) {
		case PKEY_DISABLE_ACCESS:
			tst_res(TFAIL | TERRNO,
				"Read buffer success, buffer[0] = %d", *buffer);
		break;
		case PKEY_DISABLE_WRITE:
			*buffer = 'a';
			tst_res(TFAIL | TERRNO,
				"Write buffer success, buffer[0] = %d", *buffer);
		break;
		}
		exit(0);
	}
	waitpid(pid, &status, 0);
        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV)
		tst_res(TPASS, "Child ended by %s as expected", tst_strsig(SIGSEGV));
        else
                tst_res(TFAIL, "Child: %s", tst_strstatus(status));
	tst_res(TINFO, "Remove %s from the buffer", tc->name);
	if (ltp_pkey_mprotect(buffer, size, mpa->prot, 0x0) == -1)
		tst_brk(TBROK | TERRNO, "pkey_mprotect failed");
	switch (mpa->prot) {
	case PROT_READ:
		tst_res(TPASS, "Read buffer success, buffer[0] = %d", *buffer);
	break;
	case PROT_WRITE:
		*buffer = 'a';
		tst_res(TPASS, "Write buffer success, buffer[0] = %d", *buffer);
	break;
	case PROT_READ | PROT_WRITE:
	case PROT_READ | PROT_WRITE | PROT_EXEC:
		*buffer = 'a';
		tst_res(TPASS, "Read & Write buffer success, buffer[0] = %d", *buffer);
	break;
	}
	if (fd >= 0)
		close(fd);
	munmap(buffer, size);
	if (ltp_pkey_free(pkey) == -1)
		tst_brk(TBROK | TERRNO, "pkey_free failed");
}

int  verify_pkey(unsigned int i)
{
	long unsigned int j;
	struct mmap_param *mpa;
	struct tcase *tc = &tcases[i];
	for (j = 0; j < ARRAY_SIZE(mmap_params); j++) {
		mpa = &mmap_params[j];
		pkey_test(tc, mpa);
	}
}

