#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/syscall.h>
#include <sys/types.h>

#ifdef __NR_set_robust_list
#ifndef __user
#define __user
#endif
struct robust_list {
	struct robust_list __user *next;
};
struct robust_list_head {
	struct robust_list list;
	long futex_offset;
	struct robust_list __user *list_op_pending;
};
#endif
char *TCID = "set_robust_list01";
int TST_TOTAL = 2;
void setup(void);
void cleanup(void);
int main(int argc, char **argv)
{
#ifdef __NR_set_robust_list
	int lc;
#endif
#ifdef __NR_set_robust_list
	struct robust_list_head head;
	size_t len;
	int retval;
#endif
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
#ifdef __NR_set_robust_list
	len = sizeof(struct robust_list_head);
	retval = syscall(__NR_set_robust_list, &head, -1);
	int ok = 1;
	printf("set_robust_list: retval = %ld (expected %d), errno = %d (expected %d)\n", retval, -1, errno, EINVAL);
	if(errno != EINVAL || !retval)
		ok = 0;
	retval = syscall(__NR_set_robust_list, &head, len);
	printf("set_robust_list: retval = %ld (expected %d), errno = %d (expected %d)\n", retval, 0, errno, 0);
	if(retval)
		ok = 0;
#else
	printf("set_robust_list: system call not available.\n");
#endif
	if(ok)
		printf("test succeeded\n");
	exit(EXIT_SUCCESS);
}
void setup(void)
{
}
void cleanup(void)
{
}

