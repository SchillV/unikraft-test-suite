#include "incl.h"

long page_size;

bool __attribute__((noinline)) check_stackgrow_up(void)
{
	char local_var;
	static char *addr;
       if (!addr) {
               addr = &local_var;
               return check_stackgrow_up();
       }
       return (addr < &local_var);
}

void setup(void)
{
	if (check_stackgrow_up()){
		printf("Test can't be performed with stack grows up architecture\n");
		exit(0);
	}
	page_size = getpagesize();
}

void *allocate_stack(size_t stack_size, size_t mapped_size)
{
	void *start, *stack_top, *stack_bottom;
	long reserved_size = 256 * page_size + stack_size;
	start = mmap(NULL, reserved_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	munmap(start, reserved_size);
	mmap((start + reserved_size - mapped_size), mapped_size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN, -1, 0);
	stack_top = start + reserved_size;
	stack_bottom = start + reserved_size - stack_size;
	printf("[I] start = %p, stack_top = %p, stack bottom = %p\n",
		start, stack_top, stack_bottom);
	printf("[I] mapped pages %zu, stack pages %zu\n",
	        mapped_size/page_size, stack_size/page_size);
	return stack_bottom;
}

__attribute__((noinline)) void *check_depth_recursive(void *limit)
{
	if ((off_t) &limit < (off_t) limit) {
		printf("[I] &limit = %p, limit = %p\n", &limit, limit);
		return NULL;
	}
	return check_depth_recursive(limit);
}

void grow_stack(void *stack, size_t size)
{
	pthread_t test_thread;
	pthread_attr_t attr;
	int ret;
	void *limit = stack + page_size;
	ret = pthread_attr_init(&attr);
	if (ret){
		printf("pthread_attr_init failed during setup\n");
	}
	pthread_create(&test_thread, &attr, check_depth_recursive, limit);
	pthread_join(test_thread, NULL);
	exit(0);
}

void grow_stack_success(size_t stack_size, size_t mapped_size)
{
	pid_t child_pid;
	int wstatus;
	void *stack;
	child_pid = fork();
	if (!child_pid) {
		stack = allocate_stack(stack_size, mapped_size);
		grow_stack(stack, stack_size);
	}
	wait(&wstatus);
	if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0)
		printf("Stack grows in unmapped region\ntest succeeded\n");
	else{
		printf("Child: %d\n", wstatus);
	}
}

void grow_stack_fail(size_t stack_size, size_t mapped_size)
{
	pid_t child_pid;
	int wstatus;
	void *stack;
	child_pid = fork();
	if (!child_pid) {
		stack = allocate_stack(stack_size, mapped_size);
		mmap(stack, page_size, PROT_READ | PROT_WRITE,
			  MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		printf("[I] mapped page at %p\n", stack);
		grow_stack(stack, stack_size);
	}
	wait(&wstatus);
        if (WIFSIGNALED(wstatus) && WTERMSIG(wstatus) == SIGSEGV)
			printf("Child killed by %d as expected\ntest succeeded\n", SIGSEGV);
        else
            printf("Child: %d\n", wstatus);
}

void run_test(void)
{
	size_t pthread_stack = (page_size/PTHREAD_STACK_MIN) + 1;
	size_t stack_size = 8 * pthread_stack;
	grow_stack_success(stack_size, pthread_stack);
	grow_stack_success(stack_size, stack_size/2);
	grow_stack_fail(stack_size, pthread_stack);
	grow_stack_fail(stack_size, stack_size/2);
}

void main(){
	setup();
	run_test();
}
