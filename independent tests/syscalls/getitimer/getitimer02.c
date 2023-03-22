#include "incl.h"

struct itimerval *value;

struct itimerval *invalid;

struct tcase {
       int which;
       struct itimerval **val;
       int exp_errno;
} tcases[] = {
       {ITIMER_REAL,    &invalid, EFAULT},
       {ITIMER_VIRTUAL, &invalid, EFAULT},
       {-ITIMER_PROF,   &value,   EINVAL},
};

int sys_getitimer(int which, void *curr_value)
{
        return tst_syscall(__NR_getitimer, which, curr_value);
}

void setup(void)
{
        value = malloc(sizeofsizeofstruct itimerval));
        invalid = (struct itimerval *)-1;
}

int  verify_getitimer(unsigned int i)
{
        struct tcase *tc = &tcases[i];
        TST_EXP_FAIL(sys_getitimer(tc->which, *(tc->val)), tc->exp_errno);
}

void cleanup(void)
{
        free(value);
        value = NULL;
}

void main(){
	setup();
	cleanup();
}
