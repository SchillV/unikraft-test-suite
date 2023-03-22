#include "incl.h"
#define _GNU_SOURCE
char *TCID = "mremap05";
struct test_case_t {
	char *old_address;
	char *new_address;
	int flags;
	const char *msg;
	void *exp_ret;
	int exp_errno;
	char *ret;
	void (*setup) (struct test_case_t *);
	void (*cleanup) (struct test_case_t *);
};

void setup(void);

void cleanup(void);

void setup0(struct test_case_t *);

void setup1(struct test_case_t *);

void setup2(struct test_case_t *);

void setup3(struct test_case_t *);

void setup4(struct test_case_t *);

void cleanup0(struct test_case_t *);

void cleanup1(struct test_case_t *);
struct test_case_t tdat[] = {
	{
	 .old_size = 1,
	 .new_size = 1,
	 .flags = MREMAP_FIXED,
	 .msg = "MREMAP_FIXED requires MREMAP_MAYMOVE",
	 .exp_ret = MAP_FAILED,
	 .exp_errno = EINVAL,
	 .setup = setup0,
	 .cleanup = cleanup0},
	{
	 .old_size = 1,
	 .new_size = 1,
	 .flags = MREMAP_FIXED | MREMAP_MAYMOVE,
	 .msg = "new_addr has to be page aligned",
	 .exp_ret = MAP_FAILED,
	 .exp_errno = EINVAL,
	 .setup = setup1,
	 .cleanup = cleanup0},
	{
	 .old_size = 2,
	 .new_size = 1,
	 .flags = MREMAP_FIXED | MREMAP_MAYMOVE,
	 .msg = "old/new area must not overlap",
	 .exp_ret = MAP_FAILED,
	 .exp_errno = EINVAL,
	 .setup = setup2,
	 .cleanup = cleanup0},
	{
	 .old_size = 1,
	 .new_size = 1,
	 .flags = MREMAP_FIXED | MREMAP_MAYMOVE,
	 .msg = "mremap #1",
	 .setup = setup3,
	 .cleanup = cleanup0},
	{
	 .old_size = 1,
	 .new_size = 1,
	 .flags = MREMAP_FIXED | MREMAP_MAYMOVE,
	 .msg = "mremap #2",
	 .setup = setup4,
	 .cleanup = cleanup1},
};

int pagesize;

int TST_TOTAL = sizeof(tdat) / sizeof(tdat[0]);

void free_test_area(void *p, int size)
{
	munmap(cleanup, p, size);
}

void *get_test_area(int size, int free_area)
{
	void *p;
	p = mmap(NULL, size, PROT_READ | PROT_WRITE,
		 MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (p == MAP_FAILED)
		tst_brkm(TBROK | TERRNO, cleanup, "get_test_area mmap");
	if (free_area)
		free_test_area(p, size);
	return p;
}

void test_mremap(struct test_case_t *t)
{
	t->ret = mremap(t->old_address, t->old_size, t->new_size, t->flags,
			t->new_address);
	if (t->ret == t->exp_ret) {
		if (t->ret != MAP_FAILED) {
			tst_resm(TPASS, "%s", t->msg);
			if (*(t->ret) == 0x1)
				tst_resm(TPASS, "%s value OK", t->msg);
			else
				tst_resm(TPASS, "%s value failed", t->msg);
		} else {
			if (errno == t->exp_errno)
				tst_resm(TPASS, "%s", t->msg);
			else
				tst_resm(TFAIL | TERRNO, "%s", t->msg);
		}
	} else {
		tst_resm(TFAIL, "%s ret: %p, expected: %p", t->msg,
			 t->ret, t->exp_ret);
	}
}

void setup0(struct test_case_t *t)
{
	t->old_address = get_test_area(t->old_size * pagesize, 0);
	t->new_address = get_test_area(t->new_size * pagesize, 1);
}

void setup1(struct test_case_t *t)
{
	t->old_address = get_test_area(t->old_size * pagesize, 0);
	t->new_address = get_test_area((t->new_size + 1) * pagesize, 1) + 1;
}

void setup2(struct test_case_t *t)
{
	t->old_address = get_test_area(t->old_size * pagesize, 0);
	t->new_address = t->old_address;
}

void setup3(struct test_case_t *t)
{
	t->old_address = get_test_area(t->old_size * pagesize, 0);
	t->new_address = get_test_area(t->new_size * pagesize, 1);
	t->exp_ret = t->new_address;
}

void setup4(struct test_case_t *t)
{
	t->old_address = get_test_area(t->old_size * pagesize, 0);
	t->new_address = get_test_area(t->new_size * pagesize, 0);
	t->exp_ret = t->new_address;
}

void cleanup0(struct test_case_t *t)
{
	if (t->ret == MAP_FAILED)
		free_test_area(t->old_address, t->old_size * pagesize);
	else
		free_test_area(t->ret, t->new_size * pagesize);
}

void cleanup1(struct test_case_t *t)
{
	if (t->ret == MAP_FAILED) {
		free_test_area(t->old_address, t->old_size * pagesize);
		free_test_area(t->new_address, t->new_size * pagesize);
	} else {
		free_test_area(t->ret, t->new_size * pagesize);
	}
}
int main(int ac, char **av)
{
	int lc, testno;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (testno = 0; testno < TST_TOTAL; testno++) {
			tdat[testno].setup(&tdat[testno]);
			test_mremap(&tdat[testno]);
			tdat[testno].cleanup(&tdat[testno]);
		}
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	pagesize = getpagesize();
}

void cleanup(void)
{
}

