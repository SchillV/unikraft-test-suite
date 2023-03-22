#include "incl.h"
#define TST_NO_DEFAULT_MAIN
int main(int argc, char *argv[])
{
	if (argc < 3)
		tst_brk(TFAIL, "argc is %d, expected more than two", argc);
	pid_t pid;
	struct rusage ru;
	long maxrss_self, maxrss_children;
	long consume_nr, grand_consume_nr, self_nr, child_nr;
	tst_reinit();
	if (!strcmp(argv[1], "consume")) {
		consume_nr = strtol(argv[2], 0, LONG_MAX);
		consume_mb(consume_nr);
	} else if (!strcmp(argv[1], "grand_consume")) {
		grand_consume_nr = strtol(argv[2], 0, LONG_MAX);
		pid = fork();
		if (pid == -1)
			tst_brk(TBROK, "fork failed");
		else if (pid == 0) {
			consume_mb(grand_consume_nr);
			exit(0);
		}
		tst_reap_children();
	} else if (!strcmp(argv[1], "compare")) {
		self_nr = strtol(argv[2], 0, LONG_MAX);
		child_nr = strtol(argv[3], 0, LONG_MAX);
		getrusage(RUSAGE_SELF, &ru);
		maxrss_self = ru.ru_maxrss;
		getrusage(RUSAGE_CHILDREN, &ru);
		maxrss_children = ru.ru_maxrss;
		if (is_in_delta(maxrss_self - self_nr))
			tst_res(TPASS, "initial.self ~= exec.self");
		else
			tst_res(TFAIL, "initial.self !~= exec.self");
		if (is_in_delta(maxrss_children - child_nr))
			tst_res(TPASS, "initial.children ~= exec.children");
		else
			tst_res(TFAIL, "initial.children !~= exec.children");
	}
	return 0;
}

