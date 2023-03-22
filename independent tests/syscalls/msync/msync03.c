#include "incl.h"
#define INV_SYNC	-1
#define TEMPFILE	"msync_file"
#define BUF_SIZE	256

void setup(void);

void cleanup(void);

int fd;

char *addr1;

char *addr2;

char *addr3;

char *addr4;

size_t page_sz;

struct test_case_t {
	char **addr;
	int flags;
	int exp_errno;
} test_cases[] = {
	{ &addr1, MS_INVALIDATE, EBUSY },
	{ &addr1, MS_ASYNC | MS_SYNC, EINVAL },
	{ &addr1, INV_SYNC, EINVAL },
	{ &addr2, MS_SYNC, EINVAL },
	{ &addr3, MS_SYNC, EINVAL },
	{ &addr4, MS_SYNC, ENOMEM },
};

int  msync_verify(struct test_case_t *tc);
char *TCID = "msync03";
int TST_TOTAL = ARRAY_SIZE(test_cases);
int main(int ac, char **av)
{
	int i, lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			msync_verify(&test_cases[i]);
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	size_t nwrite = 0;
	char write_buf[BUF_SIZE];
	struct rlimit rl;
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	TEST_PAUSE;
	page_sz = (size_t)sysconf(_SC_PAGESIZE);
	fd = open(cleanup, TEMPFILE, O_RDWR | O_CREAT, 0666);
	memset(write_buf, 'a', BUF_SIZE);
	while (nwrite < page_sz) {
		write(cleanup, 1, fd, write_buf, BUF_SIZE);
		nwrite += BUF_SIZE;
	}
	addr1 = mmap(cleanup, 0, page_sz, PROT_READ | PROT_WRITE,
			  MAP_SHARED | MAP_LOCKED, fd, 0);
	addr2 = addr1 + 1;
	getrlimit(NULL, RLIMIT_DATA, &rl);
	addr3 = (char *)rl.rlim_max;
	addr4 = sbrk(0) + (4 * page_sz);
}

int  msync_verify(struct test_case_t *tc)
{
msync(*(tc->addr), page_sz, tc->flags);
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL, "msync succeeded unexpectedly");
		return;
	}
	if (TEST_ERRNO == tc->exp_errno) {
		tst_resm(TPASS | TTERRNO, "msync failed as expected");
	} else {
		tst_resm(TFAIL | TTERRNO,
			 "msync failed unexpectedly; expected: "
			 "%d - %s", tc->exp_errno,
			 strerror(tc->exp_errno));
	}
}

void cleanup(void)
{
	if (addr1 && munmap(addr1, page_sz) < 0)
		tst_resm(TWARN | TERRNO, "munmap() failed");
	if (fd > 0 && close(fd) < 0)
		tst_resm(TWARN | TERRNO, "close() failed");
	tst_rmdir();
}

