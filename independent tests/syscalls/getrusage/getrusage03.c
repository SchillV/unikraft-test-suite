#include "incl.h"
#define TESTBIN "getrusage03_child"

struct rusage ru;

long maxrss_init;

const char *const resource[] = {
	TESTBIN,
	NULL,
};

void inherit_fork1(void)
{
	getrusage(RUSAGE_SELF, &ru);
	maxrss_init = ru.ru_maxrss;
	if (!fork()) {
		getrusage(RUSAGE_SELF, &ru);
		if (is_in_delta(maxrss_init - ru.ru_maxrss))
			tst_res(TPASS, "initial.self ~= child.self");
		else
			tst_res(TFAIL, "child.self = %li, expected %li",
				ru.ru_maxrss, maxrss_init);
		exit(0);
	}
	tst_reap_children();
}

void inherit_fork2(void)
{
	getrusage(RUSAGE_CHILDREN, &ru);
	if (is_in_delta(ru.ru_maxrss - 102400))
		tst_res(TPASS, "initial.children ~= 100MB");
	else
		tst_res(TFAIL, "initial.children = %li, expected %i",
			ru.ru_maxrss, 102400);
	if (!fork()) {
		getrusage(RUSAGE_CHILDREN, &ru);
		if (!ru.ru_maxrss)
			tst_res(TPASS, "child.children == 0");
		else
			tst_res(TFAIL, "child.children = %li, expected %i",
				ru.ru_maxrss, 0);
		exit(0);
	}
	tst_reap_children();
}

void grandchild_maxrss(void)
{
	if (!fork())
		execlp("getrusage03_child", "getrusage03_child",
			    "grand_consume", "300", NULL);
	tst_reap_children();
	getrusage(RUSAGE_CHILDREN, &ru);
	if (is_in_delta(ru.ru_maxrss - 307200))
		tst_res(TPASS, "child.children ~= 300MB");
	else
		tst_res(TFAIL, "child.children = %li, expected %i",
			ru.ru_maxrss, 307200);
}

void zombie(void)
{
	getrusage(RUSAGE_CHILDREN, &ru);
	maxrss_init = ru.ru_maxrss;
	pid_t pid = fork();
	if (!pid)
		execlp("getrusage03_child", "getrusage03_child",
			    "consume", "400", NULL);
	TST_PROCESS_STATE_WAIT(pid, 'Z', 0);
	getrusage(RUSAGE_CHILDREN, &ru);
	if (is_in_delta(ru.ru_maxrss - maxrss_init))
		tst_res(TPASS, "initial.children ~= pre_wait.children");
	else
		tst_res(TFAIL, "pre_wait.children = %li, expected %li",
			ru.ru_maxrss, maxrss_init);
	tst_reap_children();
	getrusage(RUSAGE_CHILDREN, &ru);
	if (is_in_delta(ru.ru_maxrss - 409600))
		tst_res(TPASS, "post_wait.children ~= 400MB");
	else
		tst_res(TFAIL, "post_wait.children = %li, expected %i",
			ru.ru_maxrss, 409600);
}

void sig_ign(void)
{
	signal(SIGCHLD, SIG_IGN);
	getrusage(RUSAGE_CHILDREN, &ru);
	maxrss_init = ru.ru_maxrss;
	pid_t pid = fork();
	if (!pid)
		execlp("getrusage03_child", "getrusage03_child",
			    "consume", "500", NULL);
	TST_PROCESS_EXIT_WAIT(pid, 0);
	getrusage(RUSAGE_CHILDREN, &ru);
	if (is_in_delta(ru.ru_maxrss - maxrss_init))
		tst_res(TPASS, "initial.children ~= after_zombie.children");
	else
		tst_res(TFAIL, "after_zombie.children = %li, expected %li",
			ru.ru_maxrss, maxrss_init);
	signal(SIGCHLD, SIG_DFL);
}

void inherit_exec(void)
{
	if (!fork()) {
		char str_maxrss_self[BUFSIZ], str_maxrss_child[BUFSIZ];
		getrusage(RUSAGE_SELF, &ru);
		sprintf(str_maxrss_self, "%ld", ru.ru_maxrss);
		getrusage(RUSAGE_CHILDREN, &ru);
		sprintf(str_maxrss_child, "%ld", ru.ru_maxrss);
		execlp("getrusage03_child", "getrusage03_child",
			    "compare", str_maxrss_self, str_maxrss_child, NULL);
	}
	tst_reap_children();
}
void (*testfunc_list[])(void) = {
	inherit_fork1, inherit_fork2, grandchild_maxrss,
	zombie, sig_ign, inherit_exec
};

void run(unsigned int i)
{
	if (!fork()) {
		if (!fork()) {
			consume_mb(100);
			exit(0);
		}
		wait(NULL);
		testfunc_list[i]();
	}
}

void main(){
	setup();
	cleanup();
}
