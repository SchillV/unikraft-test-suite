#include "incl.h"

void sighandler(int sig);

void setup(void);

void cleanup(void);

void testfunc_protnone(void);

void testfunc_protexec(void);

void (*testfunc[])(void) = { testfunc_protnone, testfunc_protexec };
char *TCID = "mprotect04";
int TST_TOTAL = sizeof(testfunc) / sizeof(testfunc[0]);

volatile int sig_caught;

unsigned int page_sz;
typedef void (*func_ptr_t)(void);
int main(int ac, char **av)
{
	int lc;
	int i;
	setup();
	for (i = 0; i < TST_TOTAL; i++)
		(*testfunc[i])();
	cleanup();
}

void sighandler(int sig)
{
	sig_caught = sig;
	siglongjmp(env, 1);
}

void setup(void)
{
	page_sz = getpagesize();
}

void testfunc_protnone(void)
{
	char *addr;
	sig_caught = 0;
	addr = mmap(cleanup, 0, page_sz, PROT_READ | PROT_WRITE,
					 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
mprotect(addr, page_sz, PROT_NONE);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL | TTERRNO, "mprotect failed");
	} else {
		if (sigsetjmp(env, 1) == 0)
			addr[0] = 1;
		switch (sig_caught) {
		case SIGSEGV:
			tst_resm(TPASS, "test PROT_NONE for mprotect success");
		break;
		case 0:
			tst_resm(TFAIL, "test PROT_NONE for mprotect failed");
		break;
		default:
			tst_brkm(TBROK, cleanup,
			         "received an unexpected signal: %d",
			         sig_caught);
		}
	}
	munmap(cleanup, addr, page_sz);
}

void exec_func(void)
{
	return;
}

int page_present(void *p)
{
	int fd;
	fd = open(cleanup, "page_present", O_WRONLY|O_CREAT, 0644);
write(fd, p, 1);
	close(cleanup, fd);
	if (TEST_RETURN >= 0)
		return 1;
	if (TEST_ERRNO != EFAULT)
		tst_brkm(TBROK | TTERRNO, cleanup, "page_present write");
	return 0;
}

void clear_cache(void *start, int len)
{
#if HAVE_BUILTIN_CLEAR_CACHE == 1
	__builtin___clear_cache(start, start + len);
#else
	tst_brkm(TCONF, cleanup,
		"compiler doesn't have __builtin___clear_cache()");
#endif
}
#if defined(__powerpc64__) && (!defined(_CALL_ELF) || _CALL_ELF < 2)
#define USE_FUNCTION_DESCRIPTORS
#endif
#ifdef USE_FUNCTION_DESCRIPTORS
typedef struct {
	uintptr_t entry;
	uintptr_t toc;
	uintptr_t env;
} func_descr_t;
#endif

void *get_func(void *mem, uintptr_t *func_page_offset)
{
	uintptr_t page_sz = getpagesize();
	uintptr_t page_mask = ~(page_sz - 1);
	void *func_copy_start, *page_to_copy;
	void *mem_start = mem;
#ifdef USE_FUNCTION_DESCRIPTORS
	func_descr_t *opd =  (func_descr_t *)&exec_func;
	func_copy_start = mem + *func_page_offset;
	page_to_copy = (void *)((uintptr_t)opd->entry & page_mask);
#else
	func_copy_start = mem + *func_page_offset;
	page_to_copy = (void *)((uintptr_t)&exec_func & page_mask);
#endif
	tst_resm(TINFO, "exec_func: %p, page_to_copy: %p",
		&exec_func, page_to_copy);
	if (!page_present(page_to_copy)) {
mprotect(page_to_copy, page_sz, PROT_READ | PROT_EXEC);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL | TTERRNO,
				 "mprotect(PROT_READ|PROT_EXEC) failed");
			return NULL;
		}
		if (!page_present(page_to_copy))
			tst_brkm(TBROK, cleanup, "page_to_copy not present");
	}
	memcpy(mem, page_to_copy, page_sz);
	clear_cache(mem_start, page_sz);
	return func_copy_start;
}

void testfunc_protexec(void)
{
	func_ptr_t func;
	uintptr_t func_page_offset;
	void *p;
	sig_caught = 0;
	p = mmap(cleanup, 0, page_sz, PROT_READ | PROT_WRITE,
		 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#ifdef USE_FUNCTION_DESCRIPTORS
	func_descr_t opd;
	opd.entry = (uintptr_t)get_func(p, &func_page_offset);
	func = (func_ptr_t)&opd;
#else
	func = get_func(p, &func_page_offset);
#endif
	if (!func)
		goto out;
	if (func_page_offset + 64 > page_sz) {
		munmap(cleanup, p, page_sz);
		tst_brkm(TCONF, cleanup, "func too close to page boundary, "
			"maybe your compiler ignores -falign-functions?");
	}
mprotect(p, page_sz, PROT_EXEC);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL | TTERRNO, "mprotect failed");
	} else {
		if (sigsetjmp(env, 1) == 0)
			(*func)();
		switch (sig_caught) {
		case SIGSEGV:
			tst_resm(TFAIL, "test PROT_EXEC for mprotect failed");
		break;
		case 0:
			tst_resm(TPASS, "test PROT_EXEC for mprotect success");
		break;
		default:
			tst_brkm(TBROK, cleanup,
			         "received an unexpected signal: %d",
			         sig_caught);
		}
	}
out:
	munmap(cleanup, p, page_sz);
}

void cleanup(void)
{
}

