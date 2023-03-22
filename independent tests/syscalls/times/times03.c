#include "incl.h"

volatile int timeout;

void sighandler(int signal)
{
	if (signal == SIGALRM)
		timeout = 1;
}

volatile int k;

void work(void)
{
	int i, j;
	while (!timeout)
		for (i = 0; i < 10000; i++)
			for (j = 0; j < 100; j++)
				k = i * j;
	timeout = 0;
}

void generate_utime(void)
{
	alarm(1);
	work();
}

void generate_stime(void)
{
	time_t start_time, end_time;
	struct tms buf;
	 * At least some CPU time must be used in system space. This is
	 * achieved by executing the times(2) call for
	 * at least 2 secs. This logic makes it independent
	 * of the processor speed.
	 */
	start_time = time(NULL);
	for (;;) {
		if (times(&buf) == -1)
			tst_res(TFAIL | TERRNO, "times failed");
		end_time = time(NULL);
		if ((end_time - start_time) > 2)
			return;
	}
}

int  verify_times(
{
	int pid;
	struct tms buf1, buf2, buf3;
	if (times(&buf1) == -1)
		tst_brk(TBROK | TERRNO, "times()");
	if (buf1.tms_utime > 5)
		tst_res(TFAIL, "buf1.tms_utime = %li", buf1.tms_utime);
	else
		tst_res(TPASS, "buf1.tms_utime <= 5");
	if (buf1.tms_stime > 5)
		tst_res(TFAIL, "buf1.tms_stime = %li", buf1.tms_stime);
	else
		tst_res(TPASS, "buf1.tms_stime <= 5");
	generate_utime();
	generate_stime();
	if (times(&buf2) == -1)
		tst_brk(TBROK | TERRNO, "times()");
	if (buf2.tms_utime == 0)
		tst_res(TFAIL, "buf2.tms_utime = 0");
	else
		tst_res(TPASS, "buf2.tms_utime = %li", buf2.tms_utime);
	if (buf1.tms_utime >= buf2.tms_utime) {
		tst_res(TFAIL, "buf1.tms_utime (%li) >= buf2.tms_utime (%li)",
			buf1.tms_utime, buf2.tms_utime);
	} else {
		tst_res(TPASS, "buf1.tms_utime (%li) < buf2.tms_utime (%li)",
			buf1.tms_utime, buf2.tms_utime);
	}
	if (buf2.tms_stime == 0)
		tst_res(TFAIL, "buf2.tms_stime = 0");
	else
		tst_res(TPASS, "buf2.tms_stime = %li", buf2.tms_stime);
	if (buf1.tms_stime >= buf2.tms_stime) {
		tst_res(TFAIL, "buf1.tms_stime (%li) >= buf2.tms_stime (%li)",
			buf1.tms_stime, buf2.tms_stime);
	} else {
		tst_res(TPASS, "buf1.tms_stime (%li) < buf2.tms_stime (%li)",
			buf1.tms_stime, buf2.tms_stime);
	}
	if (buf2.tms_cutime != 0)
		tst_res(TFAIL, "buf2.tms_cutime = %li", buf2.tms_cutime);
	else
		tst_res(TPASS, "buf2.tms_cutime = 0");
	if (buf2.tms_cstime != 0)
		tst_res(TFAIL, "buf2.tms_cstime = %li", buf2.tms_cstime);
	else
		tst_res(TPASS, "buf2.tms_cstime = 0");
	pid = fork();
	if (!pid) {
		generate_utime();
		generate_stime();
		exit(0);
	}
	waitpid(pid, NULL, 0);
	if (times(&buf3) == -1)
		tst_brk(TBROK | TERRNO, "times()");
	if (buf2.tms_utime > buf3.tms_utime) {
		tst_res(TFAIL, "buf2.tms_utime (%li) > buf3.tms_utime (%li)",
			buf2.tms_utime, buf3.tms_utime);
	} else {
		tst_res(TPASS, "buf2.tms_utime (%li) <= buf3.tms_utime (%li)",
			buf2.tms_utime, buf3.tms_utime);
	}
	if (buf2.tms_stime > buf3.tms_stime) {
		tst_res(TFAIL, "buf2.tms_stime (%li) > buf3.tms_stime (%li)",
			buf2.tms_stime, buf3.tms_stime);
	} else {
		tst_res(TPASS, "buf2.tms_stime (%li) <= buf3.tms_stime (%li)",
			buf2.tms_stime, buf3.tms_stime);
	}
	if (buf3.tms_cutime == 0)
		tst_res(TFAIL, "buf3.tms_cutime = 0");
	else
		tst_res(TPASS, "buf3.tms_cutime = %ld", buf3.tms_cutime);
	if (buf3.tms_cstime == 0)
		tst_res(TFAIL, "buf3.tms_cstime = 0");
	else
		tst_res(TPASS, "buf3.tms_cstime = %ld", buf3.tms_cstime);
	exit(0);
}

void do_test(void)
{
	int pid = fork();
	if (!pid)
int 		verify_times();
}

void setup(void)
{
	signal(SIGALRM, sighandler);
}

void main(){
	setup();
	cleanup();
}
