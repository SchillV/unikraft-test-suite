#include "incl.h"
#define MAP_SIZE (16 * 1024)

char pattern[MAP_SIZE];

char zero[MAP_SIZE];

const struct test_case {
	int size;
	int advise1;
	int advise2;
	char *exp;
	int grand_child;
	const char *desc;
} tcases[] = {
	{MAP_SIZE, MADV_NORMAL,	MADV_WIPEONFORK, zero,    0,
	"MADV_WIPEONFORK zeroes memory in child"},
	{0,	   MADV_NORMAL, MADV_WIPEONFORK, pattern, 0,
	"MADV_WIPEONFORK with zero length does nothing"},
	{MAP_SIZE, MADV_NORMAL, MADV_WIPEONFORK, zero,    1,
	"MADV_WIPEONFORK zeroes memory in grand-child"},
	{MAP_SIZE, MADV_WIPEONFORK, MADV_KEEPONFORK, pattern, 0,
	"MADV_KEEPONFORK will undo MADV_WIPEONFORK"},
};

void cmp_area(char *addr, const struct test_case *tc)
{
	int i;
	for (i = 0; i < tc->size; i++) {
		if (addr[i] != tc->exp[i]) {
			tst_res(TFAIL, "In PID %d, addr[%d] = 0x%02x, "
				"expected[%d] = 0x%02x", getpid(),
				i, addr[i], i, tc->exp[i]);
			break;
		}
	}
	tst_res(TPASS, "In PID %d, Matched expected pattern", getpid());
}

int set_advice(char *addr, int size, int advise)
{
madvise(addr, size, advise);
	if (TST_RET == -1) {
		if (TST_ERR == EINVAL) {
			tst_res(TCONF, "madvise(%p, %d, 0x%x) is not supported",
			addr, size, advise);
		} else {
			tst_res(TFAIL | TTERRNO, "madvise(%p, %d, 0x%x)",
			addr, size, advise);
		}
		return 1;
	}
	tst_res(TPASS, "madvise(%p, %d, 0x%x)",	addr, size, advise);
	return 0;
}

char *mem_map(void)
{
	char *ptr;
	ptr = mmap(NULL, MAP_SIZE,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0);
	memcpy(ptr, pattern, MAP_SIZE);
	return ptr;
}

void test_madvise(unsigned int test_nr)
{
	const struct test_case *tc = &tcases[test_nr];
	char *addr;
	pid_t pid;
	addr = mem_map();
	tst_res(TINFO, "%s", tc->desc);
	if (set_advice(addr, tc->size, tc->advise1))
		goto un_map;
	if (!set_advice(addr, tc->size, tc->advise2)) {
		pid = fork();
		if (!pid) {
			if (tc->grand_child) {
				pid = fork();
				if (!pid) {
					cmp_area(addr, tc);
					exit(0);
				}
			} else {
				cmp_area(addr, tc);
				exit(0);
			}
		}
		tst_reap_children();
	}
un_map:
	munmap(addr, MAP_SIZE);
}

void setup(void)
{
	unsigned int i;
	for (i = 0; i < MAP_SIZE; i++)
		pattern[i] = i % 0x03;
}

