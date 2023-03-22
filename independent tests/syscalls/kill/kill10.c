#include "incl.h"
void setup();
void help();
void cleanup();
void fork_pgrps(int pgrps_left);
void manager(int num_procs);
void fork_procs(int procs_left);
void ack_ready(int sig, siginfo_t * si, void *data);
void ack_done(int sig, siginfo_t * si, void *data);
void set_create_procs(int sig);
void graceful_exit(int sig);
void set_signal_parents(int sig);
void clear_signal_parents(int sig);
void set_confirmed_ready(int sig);
void reset_counter(int sig);
void reply_to_child(int sig, siginfo_t * si, void *data);
void wakeup(int sig);
struct pid_list_item {
	pid_t pid;
	short flag;
} *child_checklist = NULL;
int child_checklist_total = 0;
int checklist_cmp(const void *a, const void *b);
void checklist_reset(int bit);

inline int k_sigaction(int sig, struct sigaction *sa, struct sigaction *osa);
char *TCID = "kill10";
int TST_TOTAL = 1;
int num_procs = 10;
int num_pgrps = 2;
int pgrps_ready = 0;
int child_signal_counter = 0;
int create_procs_flag = 0;
int signal_parents_flag = 0;
int confirmed_ready_flag = 0;
int debug_flag = 0;
pid_t mypid = 0;
char *narg, *garg, *darg;
int nflag = 0, gflag = 0, dflag = 0;
option_t options[] = {
	{NULL, NULL, NULL}
};
int main(int ac, char **av)
{
	int lc;
	int cnt;
	tst_parse_opts(ac, av, options, &help);
	if (nflag) {
		if (sscanf(narg, "%i", &num_procs) != 1) {
			tst_brkm(TBROK, NULL, "-n option arg is not a number");
		}
	}
	if (gflag) {
		if (sscanf(garg, "%i", &num_pgrps) != 1) {
			tst_brkm(TBROK, NULL, "-g option arg is not a number");
		}
	}
	if (dflag) {
		if (sscanf(darg, "%i", &debug_flag) != 1) {
			tst_brkm(TBROK, NULL, "-d option arg is not a number");
		}
	}
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		child_signal_counter = 0;
		pgrps_ready = 0;
		checklist_reset(0x03);
		for (cnt = 0; cnt < child_checklist_total; ++cnt) {
			if (debug_flag >= 2)
				printf("%d: test_loop, SIGUSR1 -> %d\n",
				       mypid, -child_checklist[cnt].pid);
			kill(-child_checklist[cnt].pid, SIGUSR1);
		}
		while (child_signal_counter < num_pgrps) {
			alarm(1);
			if (debug_flag >= 2)
				printf("%d: Master pausing for done (%d/%d)\n",
				       mypid, child_signal_counter, num_pgrps);
			pause();
		}
		tst_resm(TPASS, "All %d pgrps received their signals",
			 child_signal_counter);
	}
	cleanup();
	tst_exit();
}
void help(void)
{
	printf("  -g n    Create n process groups (default: %d)\n", num_pgrps);
	printf
	    ("  -n n    Create n children in each process group (default: %d)\n",
	     num_procs);
	printf("  -d n    Set debug level to n (default: %d)\n", debug_flag);
}
void setup(void)
{
	struct sigaction sa;
	int i;
	 * unexpected signals like SIGSEGV.
	 */
	tst_sig(FORK, DEF_HANDLER, cleanup);
	 * fork the test with the -c option.  You want to make sure you do this
	 * before you create your temporary directory.
	 */
	TEST_PAUSE;
	mypid = getpid();
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (debug_flag >= 1)
		printf("%d: setting SIGTRAP -> SIG_DFL\n", mypid);
	k_sigaction(SIGTRAP, &sa, NULL);
	if (debug_flag >= 1)
		printf("%d: setting SIGCONT -> SIG_DFL\n", mypid);
	k_sigaction(SIGCONT, &sa, NULL);
	sa.sa_handler = set_create_procs;
	if (debug_flag >= 4)
		printf("%d: setting SIGALRM -> set_create_procs\n", mypid);
	k_sigaction(SIGALRM, &sa, NULL);
	sa.sa_handler = NULL;
	sa.sa_sigaction = ack_ready;
	sa.sa_flags = SA_SIGINFO;
	if (debug_flag >= 1)
		printf("%d: setting SIGUSR1 -> ack_ready\n", mypid);
	k_sigaction(SIGUSR1, &sa, NULL);
	fork_pgrps(num_pgrps);
	if (debug_flag)
		printf("Master: %d\n", mypid);
	while (pgrps_ready < num_pgrps) {
		if (debug_flag >= 3)
			printf
			    ("%d: Master pausing for Managers to check in (%d/%d)\n",
			     mypid, pgrps_ready, num_pgrps);
		 * We might receive the signal from the (last manager) before
		 * we issue a pause. In that case we might hang even if we have
		 * all the managers reported in. So set an alarm so that we can
		 * wake up.
		 */
		alarm(1);
		pause();
	}
	checklist_reset(0x03);
	if (debug_flag) {
		printf("Managers: \n");
		for (i = 0; i < num_pgrps; i++) {
			printf("%d ", child_checklist[i].pid);
		}
		printf("\n");
	}
	sa.sa_handler = wakeup;
	if (debug_flag >= 4)
		printf("%d: setting SIGALRM -> wakeup\n", mypid);
	k_sigaction(SIGALRM, &sa, NULL);
	sa.sa_handler = NULL;
	sa.sa_sigaction = ack_done;
	sa.sa_flags = SA_SIGINFO;
	if (debug_flag >= 1)
		printf("%d: setting SIGUSR2 -> ack_done\n", mypid);
	k_sigaction(SIGUSR2, &sa, NULL);
}
void ack_ready(int sig, siginfo_t * si, void *data)
{
	struct pid_list_item findit, *result;
	findit.pid = si->si_pid;
	result = bsearch(&findit, child_checklist, child_checklist_total,
			 sizeof(*child_checklist), checklist_cmp);
	if (result) {
		if (!(result->flag & 0x01)) {
			if (debug_flag >= 3)
				printf("%d: ack_ready, SIGUSR1 -> %d\n", mypid,
				       si->si_pid);
			kill(si->si_pid, SIGUSR1);
			result->flag = result->flag | 0x01;
			++pgrps_ready;
		} else {
			if (debug_flag >= 3)
				printf("%d: ack_ready, already acked %d\n",
				       mypid, si->si_pid);
		}
	} else {
		printf("received unexpected signal %d from %d",
		       sig, si->si_pid);
	}
}
void ack_done(int sig, siginfo_t * si, void *data)
{
	struct pid_list_item findit, *result;
	findit.pid = si->si_pid;
	result = bsearch(&findit, child_checklist, child_checklist_total,
			 sizeof(*child_checklist), checklist_cmp);
	if (result) {
		if (!(result->flag & 0x02)) {
			if (debug_flag >= 3)
				printf("%d: ack_done, SIGHUP -> %d\n", mypid,
				       si->si_pid);
			kill(si->si_pid, SIGHUP);
			++child_signal_counter;
			result->flag = result->flag | 0x02;
		} else {
			if (debug_flag >= 3)
				printf("%d: ack_done, already told %d\n", mypid,
				       si->si_pid);
		}
	} else {
		printf("received unexpected signal %d from %d",
		       sig, si->si_pid);
	}
}
void cleanup(void)
{
	int i;
	for (i = 0; i < num_pgrps; ++i) {
		kill(-child_checklist[i].pid, SIGQUIT);
		waitpid(child_checklist[i].pid, NULL, 0);
	}
	free(child_checklist);
}
void fork_pgrps(int pgrps_left)
{
	pid_t child;
	if (!(child_checklist = calloc(pgrps_left, sizeof(*child_checklist)))) {
		tst_brkm(TBROK, cleanup,
			 "%d: couldn't calloc child_checklist, errno=%d : %s",
			 mypid, errno, strerror(errno));
	}
	child_checklist_total = 0;
	while (pgrps_left) {
		if (debug_flag >= 1)
			printf("%d: forking new Manager\n", mypid);
		switch (child = fork()) {
		case -1:
			tst_brkm(TBROK | TERRNO, cleanup,
				 "fork() failed in fork_pgrps(%d)", pgrps_left);
			break;
		case 0:
			mypid = getpid();
			free(child_checklist);
			child_checklist = NULL;
			manager(num_procs);
			break;
		default:
			child_checklist[child_checklist_total++].pid = child;
			setpgid(child, child);
			if (debug_flag >= 3)
				printf("%d: fork_pgrps, SIGALRM -> %d\n", mypid,
				       child);
			kill(child, SIGALRM);
		}
		--pgrps_left;
	}
	qsort(child_checklist, child_checklist_total, sizeof(*child_checklist),
	      checklist_cmp);
}
void set_create_procs(int sig)
{
	if (debug_flag >= 3)
		printf("%d: Manager cleared to fork\n", getpid());
	create_procs_flag++;
	return;
}
void manager(int num_procs)
{
	struct sigaction sa;
	while (!create_procs_flag) {
		alarm(1);
		if (debug_flag >= 3)
			printf("%d: Manager pausing, not cleared to fork\n",
			       mypid);
		pause();
	}
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (debug_flag >= 4)
		printf("%d: setting SIGHUP -> SIG_IGN\n", mypid);
	k_sigaction(SIGHUP, &sa, NULL);
	sa.sa_handler = wakeup;
	if (debug_flag >= 4)
		printf("%d: setting SIGALRM -> wakeup\n", mypid);
	k_sigaction(SIGALRM, &sa, NULL);
	sa.sa_handler = graceful_exit;
	if (debug_flag >= 4)
		printf("%d: setting SIGQUIT -> graceful_exit\n", mypid);
	k_sigaction(SIGQUIT, &sa, NULL);
	sa.sa_handler = set_signal_parents;
	sigfillset(&sa.sa_mask);
	if (debug_flag >= 7)
		printf("%d: setting SIGUSR1 -> set_signal_parents\n", mypid);
	k_sigaction(SIGUSR1, &sa, NULL);
	sa.sa_handler = clear_signal_parents;
	if (debug_flag >= 7)
		printf("%d: setting SIGUSR2 -> clear_signal_parents\n", mypid);
	k_sigaction(SIGUSR2, &sa, NULL);
	fork_procs(num_procs);
	sa.sa_handler = wakeup;
	if (debug_flag >= 4)
		printf("%d: setting SIGALRM -> wakeup\n", mypid);
	k_sigaction(SIGALRM, &sa, NULL);
	sa.sa_handler = set_confirmed_ready;
	if (debug_flag >= 4)
		printf("%d: setting SIGUSR1 -> set_confirmed_ready\n", mypid);
	k_sigaction(SIGUSR1, &sa, NULL);
	sa.sa_handler = reset_counter;
	if (debug_flag >= 4)
		printf("%d: setting SIGHUP -> reset_counter\n", mypid);
	k_sigaction(SIGHUP, &sa, NULL);
	sa.sa_handler = NULL;
	sa.sa_sigaction = reply_to_child;
	sa.sa_flags = SA_SIGINFO;
	if (debug_flag >= 4)
		printf("%d: setting SIGUSR2 -> reply_to_child\n", mypid);
	k_sigaction(SIGUSR2, &sa, NULL);
	while (!confirmed_ready_flag) {
		if (debug_flag >= 3)
			printf("%d: Manager, SIGUSR1 -> %d\n", mypid,
			       getppid());
		if (kill(getppid(), SIGUSR1) == -1) {
			printf("%d: Couldn't signal master (%d) that we're "
			       "ready. %d: %s",
			       mypid, getppid(), errno, strerror(errno));
			exit(errno);
		}
		usleep(100);
	}
	while (1) {
		alarm(1);
		if (debug_flag >= 5)
			printf("%d: Manager pausing (%d/%d)\n",
			       mypid, child_signal_counter, num_procs);
		pause();
		if (child_signal_counter >= num_procs) {
			confirmed_ready_flag = 0;
			printf("%d: All %d children reported in\n",
			       mypid, child_signal_counter);
			while (child_signal_counter) {
				if (debug_flag >= 3)
					printf("%d: Manager, SIGUSR2 -> %d\n",
					       mypid, getppid());
				if (kill(getppid(), SIGUSR2) == -1) {
					printf("%d: Couldn't signal master "
					       "(%d) that we're ready. %d: %s\n",
					       mypid, getppid(), errno,
					       strerror(errno));
					exit(errno);
				}
				usleep(100);
			}
		}
	}
}
void graceful_exit(int sig)
{
	exit(0);
}
void set_signal_parents(int sig)
{
	if (debug_flag >= 8)
		printf("%d: Child start signaling\n", mypid);
	signal_parents_flag = 1;
}
void clear_signal_parents(int sig)
{
	if (debug_flag >= 8)
		printf("%d: Child stop signaling\n", mypid);
	signal_parents_flag = 0;
}
void set_confirmed_ready(int sig)
{
	if (debug_flag >= 3)
		printf("%d: Manager confirmed ready\n", mypid);
	confirmed_ready_flag = 1;
}
void reset_counter(int sig)
{
	checklist_reset(0xFF);
	child_signal_counter = 0;
	if (debug_flag >= 3)
		printf("%d: reset_counter\n", mypid);
}
void reply_to_child(int sig, siginfo_t * si, void *data)
{
	struct pid_list_item findit, *result;
	findit.pid = si->si_pid;
	result = bsearch(&findit, child_checklist, child_checklist_total,
			 sizeof(*child_checklist), checklist_cmp);
	if (result) {
		if (!result->flag) {
			if (debug_flag >= 6)
				printf("%d: reply_to_child, SIGUSR1 -> %d\n",
				       mypid, si->si_pid);
			kill(si->si_pid, SIGUSR2);
			++child_signal_counter;
			result->flag = 1;
		} else {
			if (debug_flag >= 6)
				printf("%d: reply_to_child, already told %d\n",
				       mypid, si->si_pid);
		}
	} else {
		tst_brkm(TBROK, cleanup,
			 "received unexpected signal from %d", si->si_pid);
	}
}
void wakeup(int sig)
{
	return;
}
void fork_procs(int procs_left)
{
	pid_t child;
	if (!(child_checklist = calloc(procs_left, sizeof(*child_checklist)))) {
		tst_brkm(TBROK, cleanup,
			 "%d: couldn't calloc child_checklist, errno=%d : %s",
			 mypid, errno, strerror(errno));
	}
	child_checklist_total = 0;
	signal_parents_flag = 0;
	while (procs_left) {
		if (debug_flag >= 4)
			printf("%d: forking new child\n", mypid);
		switch (child = fork()) {
		case -1:
			tst_brkm(TBROK | TERRNO, cleanup,
				 "fork() failed in fork_procs(%d)", procs_left);
			break;
		case 0:
			mypid = getpid();
			while (1) {
				if (debug_flag >= 8)
					printf("%d: child pausing\n", mypid);
				 * If we have already received the signal, we dont
				 * want to pause for it !
				 */
				while (!signal_parents_flag) {
					alarm(2);
					pause();
				}
				while (signal_parents_flag) {
					if (debug_flag >= 6)
						printf("%d: child, SIGUSR2 "
						       "-> %d\n",
						       mypid, getppid());
					if (kill(getppid(), SIGUSR2) == -1) {
						printf("%d: kill(ppid:%d, "
						       "SIGUSR2) failed. %d: %s",
						       mypid, getppid(), errno,
						       strerror(errno));
						exit(errno);
					}
					usleep(100);
				}
			}
			break;
		default:
			child_checklist[child_checklist_total++].pid = child;
		}
		procs_left--;
	}
	qsort(child_checklist, child_checklist_total, sizeof(*child_checklist),
	      checklist_cmp);
}
int checklist_cmp(const void *a, const void *b)
{
	const struct pid_list_item *pa = (const struct pid_list_item *)a;
	const struct pid_list_item *pb = (const struct pid_list_item *)b;
	return (pa->pid > pb->pid) - (pa->pid < pb->pid);
}
void checklist_reset(int bit)
{
	int i;
	for (i = 0; i < child_checklist_total; i++) {
		child_checklist[i].flag = child_checklist[i].flag & (~bit);
	}
}

inline int k_sigaction(int sig, struct sigaction *sa, struct sigaction *osa)
{
	int ret;
	if ((ret = sigaction(sig, sa, osa)) == -1) {
		tst_brkm(TBROK | TERRNO, cleanup, "sigaction(%d, ...) failed",
			 sig);
	}
	return ret;
}

