#include "incl.h"
char *TCID = "mprotect01";
int TST_TOTAL = 3;
struct test_case {
	void *addr;
	int len;
	int prot;
	int error;
	void (*setupfunc) (struct test_case *self);
};

void cleanup(void);

void setup(void);

void setup1(struct test_case *self);

void setup2(struct test_case *self);

void setup3(struct test_case *self);

int fd;
struct test_case TC[] = {
	{NULL, 0, PROT_READ, ENOMEM, setup1},
	 * Check for EINVAL by passing a pointer which is not a
	 * multiple of PAGESIZE.
	 */
	{NULL, 1024, PROT_READ, EINVAL, setup2},
	 * Check for EACCES by trying to mark a section of memory
	 * which has been mmap'ed as read-only, as PROT_WRITE
	 */
	{NULL, 0, PROT_WRITE, EACCES, setup3}
};
int main(int ac, char **av)
{
	int lc;
	int i;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++) {
			if (TC[i].setupfunc != NULL)
				TC[i].setupfunc(&TC[i]);
tst_syscall(__NR_mprotect, TC[i].addr, TC[i].len, TC[i].prot);
			if (TEST_RETURN != -1) {
				tst_resm(TFAIL, "call succeeded unexpectedly");
				continue;
			}
			if (TEST_ERRNO == TC[i].error) {
				tst_resm(TPASS, "expected failure - "
					 "errno = %d : %s", TEST_ERRNO,
					 strerror(TEST_ERRNO));
			} else {
				tst_resm(TFAIL, "unexpected error - %d : %s - "
					 "expected %d", TEST_ERRNO,
					 strerror(TEST_ERRNO), TC[i].error);
			}
		}
	}
	cleanup();
	tst_exit();
}

void setup1(struct test_case *self)
{
	self->len = getpagesize() + 1;
}

void setup2(struct test_case *self)
{
	self->addr = malloc(getpagesize());
	if (self->addr == NULL)
		tst_brkm(TINFO, cleanup, "malloc failed");
	self->addr++;
}

void setup3(struct test_case *self)
{
	fd = open(cleanup, "/dev/zero", O_RDONLY);
	self->len = getpagesize();
	 * mmap the PAGESIZE bytes as read only.
	 */
	self->addr = mmap(0, self->len, PROT_READ, MAP_SHARED, fd, 0);
	if (self->addr == MAP_FAILED)
		tst_brkm(TBROK, cleanup, "mmap failed");
}

void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
}

void cleanup(void)
{
	close(fd);
}

