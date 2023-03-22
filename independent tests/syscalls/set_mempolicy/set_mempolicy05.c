#include "incl.h"

unsigned int i;

int sys_ret;

volatile char *stack_ptr;

void run(void)
{
#ifdef __powerpc__
	register long sys_num __asm__("r0");
	register long mode __asm__("r3");
	register long node_mask_ptr __asm__("r4");
	register long node_mask_sz __asm__("r5");
#else
	const int sys_num = 276;
	const int mode;
	const int node_mask_ptr = UINT_MAX;
	const int node_mask_sz = UINT_MAX;
#endif
	char stack_pattern[0x400];
	stack_ptr = stack_pattern;
	memset(stack_pattern, 0xA5, sizeof(stack_pattern));
	tst_res(TINFO, "stack pattern is in %p-%p", stack_ptr, stack_ptr + 0x400);
#ifdef __powerpc__
	sys_num = 261;
	mode = 0;
	node_mask_ptr = ~0UL;
	node_mask_sz = ~0UL;
	asm volatile (
		"addi 1,1,1024\n\t"
		"sc\n\t"
		"addi 1,1,-1024\n\t" :
		"+r"(sys_num), "+r"(mode), "+r"(node_mask_ptr), "+r"(node_mask_sz) :
		:
		"memory", "cr0", "r6", "r7", "r8", "r9", "r10", "r11", "r12");
	sys_ret = mode;
#endif
#ifdef __i386__
	asm volatile (
		"add $0x400, %%esp\n\t"
		"int $0x80\n\t"
		"sub $0x400, %%esp\n\t" :
		"=a"(sys_ret) :
		"a"(sys_num), "b"(mode), "c"(node_mask_ptr), "d"(node_mask_sz) :
		"memory");
	sys_ret = -sys_ret;
#endif
	for (i = 0; i < sizeof(stack_pattern); i++) {
		if (stack_ptr[i] != (char)0xA5) {
			tst_brk(TFAIL,
				"User stack was overwritten with something at %d", i);
		}
	}
	switch (sys_ret) {
	case EFAULT:
		tst_res(TPASS,
			"set_mempolicy returned EFAULT (compat assumed)");
		break;
	case EINVAL:
		tst_res(TCONF,
			"set_mempolicy returned EINVAL (non compat assumed)");
		break;
	default:
		tst_res(TFAIL,
			"set_mempolicy should fail with EFAULT or EINVAL, instead returned %ld",
			(long)sys_ret);
	}
}

void main(){
	setup();
	cleanup();
}
