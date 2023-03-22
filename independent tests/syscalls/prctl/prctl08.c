#include "incl.h"
#define PROC_TIMERSLACK_PATH "/proc/self/timerslack_ns"

void check_reset_timerslack(char *message);

void check_get_timerslack(char *message, unsigned long value);

void check_inherit_timerslack(char *message, unsigned long value);

unsigned long origin_value;

struct tcase {
	void (*func_check)();
	unsigned long setvalue;
	unsigned long expvalue;
	char message[50];
} tcases[] = {
	{check_reset_timerslack, 0, 50000, "Reset"},
	{check_get_timerslack, 1, 1, "Min"},
	{check_get_timerslack, 70000, 70000, "Middle"},
	{check_get_timerslack, INT_MAX, INT_MAX, "Max"},
	{check_inherit_timerslack, 70000, 70000, "Child process"},
};

int proc_flag = 1;

void check_reset_timerslack(char *message)
{
	check_get_timerslack(message, origin_value);
}

void check_get_timerslack(char *message, unsigned long value)
{
	int ret = prctl(PR_GET_TIMERSLACK);
	if ((unsigned long)ret == value)
		printf("%s prctl(PR_GET_TIMERSLACK) got %lu expectedly\n", message, value);
	else
		printf("%s prctl(PR_GET_TIMERSLACK) expected %lu got %lu\n", message, value, ret);
}

void check_inherit_timerslack(char *message, unsigned long value)
{
	int pid;
	unsigned long current_value;
	unsigned long default_value;
	pid = fork();
	if (pid == 0) {
		current_value = prctl(PR_GET_TIMERSLACK);
		prctl(PR_SET_TIMERSLACK, 0);
		default_value = prctl(PR_GET_TIMERSLACK);
		if (current_value == value && default_value == value)
			printf("%s two timer slack values are made the same as the current value(%lu) of the creating thread.", message, value);
		else
			printf( "%s current_value is %lu, default value is %lu, the parent current value is %lu",message, current_value, default_value, value);
	}
}

int  verify_prctl(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	int ret = prctl(PR_SET_TIMERSLACK, tc->setvalue);
	if (ret == -1) {
		printf("prctl(PR_SET_TIMERSLACK, %lu) failed, error number %d\n", tc->setvalue, errno);
		return 0;
	}
	printf("prctl(PR_SET_TIMERSLACK, %lu) succeed\n", tc->setvalue);
	tc->func_check(tc->message, tc->expvalue);
	return 1;
}

void setup(void)
{
	if (access(PROC_TIMERSLACK_PATH, F_OK) == -1) {
		printf("proc doesn't support timerslack_ns interface\n");
		proc_flag = 0;
	}
	int ret = prctl(PR_GET_TIMERSLACK);
	printf("[I] current timerslack value is %lu\n", ret);
}

void main(){
	int ok=1;
	setup();
	for(int i=0;i<5;i++)
		if(verify_prctl(i)==0)
			ok=0;
	if(ok)
		printf("test succeeded\n");
}
