#include "incl.h"

volatile int alarms_received;

struct tcase {
	char *str;
	unsigned int sec;
} tcases[] = {
	{"INT_MAX", INT_MAX},
	{"UINT_MAX/2", UINT_MAX/2},
	{"UINT_MAX/4", UINT_MAX/4},
};

int  verify_alarm(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	alarms_received = 0;
	TST_EXP_PASS(alarm(tc->sec), "alarm(%u)", tc->sec);
	TST_EXP_VAL(alarm(0), tc->sec);
	if (alarms_received == 1) {
		tst_res(TFAIL, "alarm(%u) delivered SIGALRM for seconds value %s",
				tc->sec, tc->str);
	}
}

void sighandler(int sig)
{
	if (sig == SIGALRM)
		alarms_received++;
}

void setup(void)
{
	signal(SIGALRM, sighandler);
}

