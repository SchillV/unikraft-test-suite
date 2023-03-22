#include "incl.h"
#define PAGES	8
#define HPAGES	(PAGES / 2)

size_t pgsz;

unsigned char vec[PAGES+1];

struct tcase {
	size_t populate_pages;
	size_t lock_pages;
	size_t offset;
	size_t exp_vmlcks;
	size_t exp_present_pages;
	int flag;
} tcases[] = {
	{0, 1, 0, 1, 1, 0},
	{0, PAGES, 0, PAGES, PAGES, 0},
	 * range is little more than 2 pages.
	 */
	{0, 2, 1, 3, 3, 0},
	 * range is little less than 2 pages.
	 */
	{0, 2, -1, 2, 2, 0},
	 * pages populated by data.
	 */
	{0, 1, 0, 1, 0, MLOCK_ONFAULT},
	{HPAGES, PAGES, 0, PAGES, HPAGES, MLOCK_ONFAULT},
	{1, HPAGES, 1, HPAGES + 1, 1, MLOCK_ONFAULT},
	{HPAGES, HPAGES, -1, HPAGES, HPAGES, MLOCK_ONFAULT},
};

size_t check_locked_pages(char *addr, size_t len, size_t num_pgs)
{
	size_t n;
	size_t act_pages = 0;
	mincore(addr, len, vec);
	for (n = 0; n < num_pgs; n++) {
		if (vec[n] & 1)
			act_pages++;
	}
	return act_pages;
}

int  verify_mlock2(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	size_t bsize, asize, act_vmlcks, act_pgs;
	char *addr;
	addr = mmap(NULL, PAGES * pgsz, PROT_WRITE,
			 MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (tc->populate_pages)
		memset(addr, 0, tc->populate_pages * pgsz);
	file_lines_scanf("/proc/self/status", "VmLck: %ld", &bsize);
tst_syscall(__NR_mlock2, addr, tc->lock_pages * pgsz + tc->offse;
			 tc->flag));
	file_lines_scanf("/proc/self/status", "VmLck: %ld", &asize);
	if (TST_RET != 0) {
		if (tc->flag && TST_ERR == EINVAL)
			tst_res(TCONF, "mlock2() didn't support MLOCK_ONFAULT");
		else
			tst_res(TFAIL | TTERRNO, "mlock2() failed");
		goto end2;
	}
	act_vmlcks = (asize - bsize) * 1024 / pgsz;
	if (tc->exp_vmlcks != act_vmlcks) {
		tst_res(TFAIL, "VmLck showed wrong %ld pages, expected %ld",
			act_vmlcks, tc->exp_vmlcks);
		goto end1;
	}
	act_pgs = check_locked_pages(addr, PAGES * pgsz, PAGES);
	if (act_pgs != tc->exp_present_pages) {
		tst_res(TFAIL, "mlock2(%d) locked %ld pages, expected %ld",
			tc->flag, act_pgs, tc->exp_present_pages);
	} else {
		tst_res(TPASS, "mlock2(%d) succeeded in locking %ld pages",
			tc->flag, tc->exp_present_pages);
	}
end1:
	munlock(addr, tc->lock_pages * pgsz + tc->offset);
end2:
	munmap(addr, PAGES * pgsz);
}

void setup(void)
{
	pgsz = getpagesize();
}

void main(){
	setup();
	cleanup();
}
