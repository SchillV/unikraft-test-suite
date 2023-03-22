#include "incl.h"

int pagesize;

rlim_t STACK_LIMIT = 10485760;

void cleanup(void);

void setup(void);
char *TCID = "mincore01";

char *global_pointer;

unsigned char *global_vec;

int global_len;
struct test_case_t;

void setup1(struct test_case_t *tc);

void setup2(struct test_case_t *tc);

void setup3(struct test_case_t *tc);

void setup4(struct test_case_t *tc);

struct test_case_t {
	char *addr;
	size_t len;
	unsigned char *vector;
	int exp_errno;
	void (*setupfunc) (struct test_case_t *tc);
} TC[] = {
	{NULL, 0, NULL, EINVAL, setup1},
	{NULL, 0, NULL, EFAULT, setup2},
	{NULL, 0, NULL, ENOMEM, setup3},
	{NULL, 0, NULL, ENOMEM, setup4}
};

int  mincore_verify(struct test_case_t *tc);
int TST_TOTAL = ARRAY_SIZE(TC);
int main(int ac, char **av)
{
	int i, lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
int 			mincore_verify(&TC[i]);
	}
	cleanup();
	tst_exit();
}

void setup1(struct test_case_t *tc)
{
	tc->addr = global_pointer + 1;
	tc->len = global_len;
	tc->vector = global_vec;
}
void setup2(struct test_case_t *tc)
{
	unsigned char *t;
	struct rlimit limit;
	t = mmap(cleanup, NULL, global_len, PROT_READ | PROT_WRITE,
		      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	munmap(cleanup, t, global_len);
	limit.rlim_cur = STACK_LIMIT;
	limit.rlim_max = STACK_LIMIT;
	if (setrlimit(RLIMIT_STACK, &limit) != 0)
		tst_brkm(TBROK | TERRNO, cleanup, "setrlimit failed");
	tc->addr = global_pointer;
	tc->len = global_len;
	tc->vector = t;
}

void setup3(struct test_case_t *tc)
{
	tc->addr = global_pointer;
	tc->len = global_len * 2;
	munmap(cleanup, global_pointer + global_len, global_len);
	tc->vector = global_vec;
}

void setup4(struct test_case_t *tc)
{
	struct rlimit as_lim;
	getrlimit(cleanup, RLIMIT_AS, &as_lim);
	tc->addr = global_pointer;
	tc->len = as_lim.rlim_cur - (rlim_t)global_pointer + pagesize;
	tc->vector = global_vec;
}

void setup(void)
{
	char *buf;
	int fd;
	pagesize = getpagesize();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	TEST_PAUSE;
	global_len = pagesize * 2;
	buf = malloc(cleanup, global_len);
	memset(buf, 42, global_len);
	fd = open(cleanup, "mincore01", O_CREAT | O_RDWR,
		       S_IRUSR | S_IWUSR);
	write(cleanup, 1, fd, buf, global_len);
	free(buf);
	global_pointer = mmap(cleanup, NULL, global_len * 2,
				   PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	global_vec = malloc(cleanup,
				 (global_len + pagesize - 1) / pagesize);
	close(cleanup, fd);
}

int  mincore_verify(struct test_case_t *tc)
{
	if (tc->setupfunc)
		tc->setupfunc(tc);
mincore(tc->addr, tc->len, tc->vector);
	if (TEST_RETURN != -1) {
		tst_resm(TFAIL, "succeeded unexpectedly");
		return;
	}
	if (TEST_ERRNO == tc->exp_errno) {
		tst_resm(TPASS | TTERRNO, "failed as expected");
	} else {
		tst_resm(TFAIL | TTERRNO,
			"mincore failed unexpectedly; expected: "
			"%d - %s", tc->exp_errno,
			strerror(tc->exp_errno));
	}
}

void cleanup(void)
{
	free(global_vec);
	if (munmap(global_pointer, global_len) == -1)
		tst_resm(TWARN | TERRNO, "munmap failed");
	tst_rmdir();
}

