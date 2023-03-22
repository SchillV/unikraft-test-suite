#include "incl.h"
#define TIMEOUT	5

void do_child(void);

void sigproc(int sig);

volatile sig_atomic_t end;

char init_cwd[PATH_MAX];

int  verify_getcwd(
{
	int status;
	char cur_cwd[PATH_MAX];
	pid_t child;
	child = fork();
	if (child == 0)
		do_child();
	 while (1) {
		getcwd(cur_cwd, PATH_MAX);
		if (strncmp(init_cwd, cur_cwd, PATH_MAX)) {
			tst_res(TFAIL, "initial current work directory is "
				 "%s, now is %s. Bug is reproduced!",
				 init_cwd, cur_cwd);
			break;
		}
		if (end) {
			tst_res(TPASS, "Bug is not reproduced!");
			break;
		}
	}
	kill(child, SIGKILL);
	waitpid(child, &status, 0);
}

void setup(void)
{
	signal(SIGALRM, sigproc);
	alarm(TIMEOUT);
	getcwd(init_cwd, PATH_MAX);
}

void sigproc(int sig)
{
	end = sig;
}

void do_child(void)
{
	unsigned int i = 0;
	char c_name[PATH_MAX] = "testfile", n_name[PATH_MAX];
	touch(c_name, 0644, NULL);
	while (1) {
		snprintf(n_name, PATH_MAX, "testfile%u", i++);
		rename(c_name, n_name);
		strncpy(c_name, n_name, PATH_MAX);
	}
}

void main(){
	setup();
	cleanup();
}
