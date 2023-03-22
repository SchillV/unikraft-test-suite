#include "incl.h"
char *TCID = "mlock02";
#if !defined(UCLINUX)

void setup(void);

void cleanup(void);

void test_enomem1(void);

void test_enomem2(void);

void test_eperm(void);

int  mlock_verify(const 

size_t len;

struct rlimit original;

struct passwd *ltpuser;

void (*test_func[])(void) = { test_enomem1, test_enomem2, test_eperm };
int TST_TOTAL = ARRAY_SIZE(test_func);
int main(int ac, char **av)
{
	int lc, i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
			(*test_func[i])();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	ltpuser = getpwnam(cleanup, "nobody");
	len = getpagesize();
	getrlimit(cleanup, RLIMIT_MEMLOCK, &original);
}

void test_enomem1(void)
{
	void *addr;
	struct rlimit rl;
	addr = mmap(cleanup, NULL, len, PROT_READ,
			 MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	munmap(cleanup, addr, len);
int 	mlock_verify(addr, len, ENOMEM);
}

void test_enomem2(void)
{
	void *addr;
	struct rlimit rl;
	rl.rlim_max = len - 1;
	rl.rlim_cur = len - 1;
	setrlimit(cleanup, RLIMIT_MEMLOCK, &rl);
	addr = mmap(cleanup, NULL, len, PROT_READ,
			 MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	seteuid(cleanup, ltpuser->pw_uid);
int 	mlock_verify(addr, len, ENOMEM);
	seteuid(cleanup, 0);
	munmap(cleanup, addr, len);
	setrlimit(cleanup, RLIMIT_MEMLOCK, &original);
}

void test_eperm(void)
{
	void *addr;
	struct rlimit rl;
	rl.rlim_max = 0;
	rl.rlim_cur = 0;
	setrlimit(cleanup, RLIMIT_MEMLOCK, &rl);
	addr = mmap(cleanup, NULL, len, PROT_READ,
			 MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	seteuid(cleanup, ltpuser->pw_uid);
int 	mlock_verify(addr, len, EPERM);
	seteuid(cleanup, 0);
	munmap(cleanup, addr, len);
	setrlimit(cleanup, RLIMIT_MEMLOCK, &original);
}

int  mlock_verify(const 
{
mlock(addr, len);
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL, "mlock succeeded unexpectedly");
		return;
	}
	if (TEST_ERRNO != error) {
		tst_resm(TFAIL | TTERRNO,
			 "mlock didn't fail as expected; expected - %d : %s",
			 error, strerror(error));
	} else {
		tst_resm(TPASS | TTERRNO, "mlock failed as expected");
	}
}

void cleanup(void)
{
}
#else
int TST_TOTAL = 1;
int main(void)
{
	tst_brkm(TCONF, NULL, "test is not available on uClinux");
}

