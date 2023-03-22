#include "incl.h"
char *TCID = "eventfd2_03";
int TST_TOTAL = 1;
#ifndef EFD_SEMLIKE
#define EFD_SEMLIKE (1 << 0)
#endif
void cleanup(void)
{
}

int eventfd2(int count, int flags)
{
	return tst_syscall(__NR_eventfd2, count, flags);
}

void xsem_wait(int fd)
{
	u_int64_t cntr;
	if (read(fd, &cntr, sizeof(cntr)) != sizeof(cntr)) {
		perror("reading eventfd");
		exit(1);
	}
	fprintf(stdout, "[%u] wait completed on %d: count=%" PRIu64 "\n",
		getpid(), fd, cntr);
}

void xsem_post(int fd, int count)
{
	u_int64_t cntr = count;
	if (write(fd, &cntr, sizeof(cntr)) != sizeof(cntr)) {
		perror("writing eventfd");
		exit(1);
	}
}

void sem_player(int fd1, int fd2)
{
	fprintf(stdout, "[%u] posting 1 on %d\n", getpid(), fd1);
	xsem_post(fd1, 1);
	fprintf(stdout, "[%u] waiting on %d\n", getpid(), fd2);
	xsem_wait(fd2);
	fprintf(stdout, "[%u] posting 1 on %d\n", getpid(), fd1);
	xsem_post(fd1, 1);
	fprintf(stdout, "[%u] waiting on %d\n", getpid(), fd2);
	xsem_wait(fd2);
	fprintf(stdout, "[%u] posting 5 on %d\n", getpid(), fd1);
	xsem_post(fd1, 5);
	fprintf(stdout, "[%u] waiting 5 times on %d\n", getpid(), fd2);
	xsem_wait(fd2);
	xsem_wait(fd2);
	xsem_wait(fd2);
	xsem_wait(fd2);
	xsem_wait(fd2);
}

void usage(char const *prg)
{
	fprintf(stderr, "use: %s [-h]\n", prg);
}
int main(int argc, char **argv)
{
	int c, fd1, fd2, status;
	pid_t cpid_poster, cpid_waiter;
	while ((c = getopt(argc, argv, "h")) != -1) {
		switch (c) {
		default:
			usage(argv[0]);
			return 1;
		}
	}
	if ((fd1 = eventfd2(0, EFD_SEMLIKE)) == -1 ||
	    (fd2 = eventfd2(0, EFD_SEMLIKE)) == -1) {
		perror("eventfd2");
		return 1;
	}
	if ((cpid_poster = fork()) == 0) {
		sem_player(fd1, fd2);
		exit(0);
	}
	if ((cpid_waiter = fork()) == 0) {
		sem_player(fd2, fd1);
		exit(0);
	}
	waitpid(cpid_poster, &status, 0);
	waitpid(cpid_waiter, &status, 0);
	tst_exit();
}

