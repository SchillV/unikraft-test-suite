#include "incl.h"
#define PAGES 8

size_t pgsz;

size_t max_sz1, max_sz2;

char *addr, *unmapped_addr;

struct passwd *nobody;

struct tcase {
	char **taddr;
	int flag;
	size_t *max_size;
	int user;
	int exp_err;
} tcases[] = {
	{&addr, -1, NULL, 0, EINVAL},
	{&addr, 0, &max_sz1, 1, ENOMEM},
	{&addr, 0, &max_sz2, 1, EPERM},
	{&unmapped_addr, 0, NULL, 0, ENOMEM},
};

int  verify_mlock2(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct rlimit orig_limit, new_limit;
	if (tc->user) {
		getrlimit(RLIMIT_MEMLOCK, &orig_limit);
		new_limit.rlim_cur = *tc->max_size;
		new_limit.rlim_max = *tc->max_size;
		setrlimit(RLIMIT_MEMLOCK, &new_limit);
		seteuid(nobody->pw_uid);
	}
tst_syscall(__NR_mlock2, *tc->taddr, pgsz, tc->flag);
	if (TST_RET != -1) {
		tst_res(TFAIL, "mlock2() succeeded");
		munlock(*tc->taddr, pgsz);
		goto end;
	}
	if (TST_ERR != tc->exp_err) {
		tst_res(TFAIL | TTERRNO,
			"mlock2() failed unexpectedly, expected %s",
			tst_strerrno(tc->exp_err));
	} else {
		tst_res(TPASS | TTERRNO, "mlock2() failed as expected");
	}
end:
	if (tc->user) {
		seteuid(0);
		setrlimit(RLIMIT_MEMLOCK, &orig_limit);
	}
}

void setup(void)
{
	pgsz = getpagesize();
	nobody = getpwnam("nobody");
	addr = mmap(NULL, pgsz, PROT_WRITE,
			 MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	unmapped_addr = mmap(NULL, pgsz * PAGES, PROT_READ,
				  MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	munmap(unmapped_addr, pgsz * PAGES);
	unmapped_addr = unmapped_addr + pgsz * PAGES / 2;
	max_sz1 = pgsz - 1;
}

void cleanup(void)
{
	if (addr)
		munmap(addr, pgsz);
}

void main(){
	setup();
	cleanup();
}
