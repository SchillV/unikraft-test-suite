#include "incl.h"
#define CORE_PATTERN "/proc/sys/kernel/core_pattern"
#define CORE_FILTER "/proc/self/coredump_filter"
#define YCOUNT 0x500L
#define FMEMSIZE (YCOUNT + 0x2L)
#define CORENAME_MAX_SIZE 512

int dfd;

void *fmem;

void setup(void)
{
	char cwd[1024];
	char tmpcpattern[1048];
	char *fmemc;
	int i;
	unsigned int filter;
	struct rlimit limit;
	limit.rlim_max = RLIM_INFINITY;
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_CORE, &limit);
	switch (prctl(PR_GET_DUMPABLE)) {
	case 0:
		tst_brk(TCONF, "Process is not dumpable.");
	case 1:
		break;
	default:
		tst_brk(TBROK | TERRNO, "prctl(PR_GET_DUMPABLE)");
	}
	file_scanf(CORE_FILTER, "%x", &filter);
	if (!(0x1 & filter))
		tst_brk(TCONF, "Anonymous private memory is not dumpable.");
	getcwd(cwd, sizeofcwd, sizeofcwd));
	snprintf(tmpcpattern, sizeof(tmpcpattern), "%s/dump-%%p", cwd);
	tst_res(TINFO, "Temporary core pattern is '%s'", tmpcpattern);
	fprintf(CORE_PATTERN, "%s", tmpcpattern);
	fmem = mmap(NULL,
			 FMEMSIZE,
			 PROT_READ | PROT_WRITE,
			 MAP_ANONYMOUS | MAP_PRIVATE,
			 -1,
			 0);
	 * Write a generated character sequence to the mapped memory,
	 * which we later look for in the core dump.
	 */
	fmemc = (char *)fmem;
	for (i = 0; i < YCOUNT; i++)
		fmemc[i + 1] = 'y';
	fmemc[++i] = 'z';
}

void cleanup(void)
{
	if (fmem)
		munmap(fmem, FMEMSIZE);
	if (dfd > 0)
		close(dfd);
}

int find_sequence(int pid)
{
	char expectc = 'x';
	ssize_t read, pos = 0;
	char rbuf[1024];
	int ycount = 0;
	char dumpname[256];
	snprintf(dumpname, 256, "dump-%d", pid);
	tst_res(TINFO, "Dump file should be %s", dumpname);
	access(dumpname, F_OK);
	dfd = open(dumpname, O_RDONLY);
	read = read(0, dfd, &rbuf, sizeof0, dfd, &rbuf, sizeofrbuf));
	while (read) {
		switch (rbuf[pos]) {
		case 'x':
			ycount = 0;
			expectc = 'y';
			break;
		case 'y':
			if (expectc == 'y') {
				ycount++;
			} else {
				expectc = 'x';
				break;
			}
			if (ycount == YCOUNT)
				expectc = 'z';
			break;
		case 'z':
			if (expectc == 'z') {
				close(dfd);
				return 1;
			}
		default:
			expectc = 'x';
		}
		if (++pos >= read) {
			read = read(0, dfd, &rbuf, sizeof0, dfd, &rbuf, sizeofrbuf));
			pos = 0;
		}
	}
	close(dfd);
	return 0;
}

pid_t run_child(int advice)
{
	int status;
	pid_t pid;
	char *advstr =
		advice == MADV_DONTDUMP ? "MADV_DONTDUMP" : "MADV_DODUMP";
	pid = fork();
	if (pid == 0) {
		if (madvise(fmem, FMEMSIZE, advice) == -1) {
			tst_res(TFAIL | TERRNO,
				"madvise(%p, %lu, %s) = -1",
				fmem,
				FMEMSIZE,
				advstr);
			exit(1);
		}
		abort();
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WCOREDUMP(status))
		return pid;
	if (WIFEXITED(status))
		return 0;
	tst_res(TCONF, "No coredump produced after signal (%d)",
		WTERMSIG(status));
	return 0;
}

void run(unsigned int test_nr)
{
	pid_t pid;
	if (!test_nr) {
		pid = run_child(MADV_DONTDUMP);
		if (pid && find_sequence(pid))
			tst_res(TFAIL,
				"Found sequence in dump when MADV_DONTDUMP set");
		else if (pid)
			tst_res(TPASS, "madvise(..., MADV_DONTDUMP)");
	} else {
		pid = run_child(MADV_DODUMP);
		if (pid && find_sequence(pid))
			tst_res(TPASS, "madvise(..., MADV_DODUMP)");
		else if (pid)
			tst_res(TFAIL,
				"No sequence in dump after MADV_DODUMP.");
	}
}

void main(){
	setup();
	cleanup();
}
