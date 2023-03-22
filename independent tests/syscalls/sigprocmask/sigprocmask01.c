#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>
#include <bits/sigaction.h>
#include <stdlib.h>

void setup();
void cleanup();	
void sig_handler(int sig);

char *TCID = "sigprocmask01";
int TST_TOTAL = 1;
int sig_catch = 0;
struct sigaction sa_new;
sigset_t set;
sigset_t sigset2;
int main(int ac, char **av)
{
	pid_t my_pid;
	setup();
	int ret = sigprocmask(SIG_BLOCK, &set, 0);
	my_pid = getpid();
	kill(my_pid, SIGINT);
	if (ret == -1) {
		printf("sigprocmask() Failed, errno=%d\n",errno);
	} else {
		if (sig_catch) {
			printf("sigprocmask fails to change the process signal mask\n");
			return 0;
		} else {
			errno = 0;
			if (sigpending(&sigset2) == -1) {
				printf("blocked signal not in pending state, error: %d\n", errno);
				cleanup();
				return 0;
			}
			errno = 0;
			if (!sigismember(&sigset2, SIGINT)) {
				printf("sigismember() failed, error:%d\n", errno);
				cleanup();
				return 0;
			}
			errno = 0;
			if (sigprocmask(SIG_UNBLOCK, &set, 0) == -1) {
				printf("sigprocmask() failed to unblock signal, error=%d\n", errno);
				cleanup();
				return 0;
			}
			if (sig_catch) {
				printf("Functionality of sigprocmask() Successful\ntest succeeded\n");
			} else {
				printf("Functionality of sigprocmask() Failed\n");
				return 0;
			}
		}
	}
	cleanup();
}
void setup(void)
{
	if (sigemptyset(&set) == -1) {
		printf("sigemptyset() failed, errno=%d\n",errno);
		cleanup();
		exit(0);
	}
	if (sigfillset(&sigset2) == -1) {
		printf("sigfillset() failed, errno=%d\n",errno);
		cleanup();
		exit(0);
	}
	sa_new.sa_handler = sig_handler;
	if (sigaction(SIGINT, &sa_new, 0) == -1) {
		printf("sigaction() failed, errno=%d\n",errno);
		cleanup();
		exit(0);
	}
	if (sigaddset(&set, SIGINT) == -1) {
		printf("sigaddset() failed, errno=%d\n",errno);
		cleanup();
		exit(0);
	}
}
void sig_handler(int sig)
{
	sig_catch++;
}
void cleanup(void)
{
}

