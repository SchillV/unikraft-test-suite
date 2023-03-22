#include "incl.h"
#define PID_MAX 32768
#define PID_MAX_STR "32768"
#define RETURN 256
#define MAX_ITERATIONS 1000000
   expected to be smaller than the second. */

int pid_distance(pid_t first, pid_t second)
{
	return (second + PID_MAX - first) % PID_MAX;
}

void check(void)
{
	pid_t prev_pid = 0;
	pid_t pid;
	int i, distance, reaped, status, retval;
	for (i = 0; i < MAX_ITERATIONS; i++) {
		retval = i % RETURN;
		pid = fork();
		if (!pid)
			exit(retval);
		if (prev_pid) {
			distance = pid_distance(prev_pid, pid);
			if (distance == 0) {
				tst_res(TFAIL,
					"Unexpected pid sequence: prev_pid=%i, pid=%i for iteration=%i",
					prev_pid, pid, i);
				return;
			}
		}
		prev_pid = pid;
		reaped = waitpid(pid, &status, 0);
		if (reaped != pid) {
			tst_res(TFAIL,
				"Wrong pid %i returned from waitpid() expected %i",
				reaped, pid);
			return;
		}
		if (WEXITSTATUS(status) != retval) {
			tst_res(TFAIL,
				"Wrong process exit value %i expected %i",
				WEXITSTATUS(status), retval);
			return;
		}
		if (!tst_remaining_runtime()) {
			tst_res(TINFO, "Runtime exhausted, exiting...");
			break;
		}
	}
	tst_res(TPASS, "%i pids forked, all passed", i);
}

