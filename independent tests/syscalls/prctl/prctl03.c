#include "incl.h"

volatile int sigchild_recv;

void check_get_subreaper(int exp_val)
{
	int get_val;
	int ret = prctl(PR_GET_CHILD_SUBREAPER, &get_val);
	if (ret == -1) {
		printf("prctl(PR_GET_CHILD_SUBREAPER) failed, error number %d\n", errno);
		return;
	}
	if (get_val == exp_val) {
		printf("prctl(PR_GET_CHILD_SUBREAPER) got expected %d\n",
			get_val);
	} else {
		printf("prctl(PR_GET_CHILD_SUBREAPER) got %d, expected %d\n",
			get_val, exp_val);
	}
}

int  verify_prctl()
{
	int status, ret;
	pid_t pid;
	pid_t ppid = getpid();
	sigchild_recv = 0;
	ret = prctl(PR_SET_CHILD_SUBREAPER, 1);
	if (ret == -1) {
		if (errno == EINVAL) {
			printf("prctl() doesn't support PR_SET_CHILD_SUBREAPER\n");
			return 0;
		} else {
			printf("prctl(PR_SET_CHILD_SUBREAPER) failed, error number %d\n", errno);
		}
		return 0;
	}
	printf("prctl(PR_SET_CHILD_SUBREAPER) succeeded\n");
	pid = fork();
	if (!pid) {
		pid_t cpid;
		cpid = fork();
		if (!cpid) {
			if (getppid() != ppid) {
				printf("PPID of orphaned process was not reparented\n");
				return 0;
			}
			printf("PPID of orphaned process was reparented\n");
			return 1;
		}
		check_get_subreaper(0);
	}
	ret = wait(&status);
	if (ret > 0) {
		printf("wait() got orphaned process, pid %d, status %d\n", ret, status);
	} else {
		printf("wait() failed to get orphaned process, error number %d\n", errno);
	}
	if (sigchild_recv == 2)
		printf("received SIGCHLD from orphaned process\n");
	else
		printf("didn't receive SIGCHLD from orphaned process\n");
	check_get_subreaper(1);
}

void sighandler(int sig __attribute__((unused)))
{
	sigchild_recv++;
}

void setup(void)
{
	signal(SIGCHLD, sighandler);
}

void main(){
	setup();
	if(verify_prctl() == 1)
		printf("test succeeded\n");
}
