#include "incl.h"
#ifndef PAGESIZE
#define PAGESIZE 4096
#endif
#define FAILED 1

void cleanup(void);

void setup(void);
char *TCID = "mprotect03";
int TST_TOTAL = 1;
int status;
char file1[BUFSIZ];
int main(int ac, char **av)
{
	char *addr;
	int fd, pid, ok = 1;
	char *buf = "abcdefghijklmnopqrstuvwxyz";
	setup();
	if ((fd = open(file1, O_RDWR | O_CREAT, 0777)) < 0){
		printf("open failed\n");
		cleanup();
		exit(0);
	}
	write(fd, buf, strlen(buf));
	addr = mmap(0, strlen(buf), PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0);
	if (addr == MAP_FAILED){
		printf("mmap failed\n");
		cleanup();
		exit(0);
	}
	int ret = mprotect(addr, strlen(buf), PROT_READ);
	if (ret != -1) {
		if ((pid = fork()) == -1) {
			printf("fork failed\n");
			cleanup();
			exit(0);
		}
		if (pid == 0) {
			memcpy(addr, buf, strlen(buf));
			printf("[I] memcpy() did not generate SIGSEGV\n");
			exit(1);
		}
		waitpid(pid, &status, 0);
		if (WEXITSTATUS(status) != 0) {
			printf("child returned unexpected status\n");
			ok = 0;
		} else {
			printf("SIGSEGV generated as expected\n");
		}
	} else {
		printf("mprotect failed unexpectedly, errno: %d\n", errno);
		ok = 0;
	}
	munmap(addr, strlen(buf));
	close(fd);
	unlink(file1);
	cleanup();
	if(ok)
		printf("test succeeded\n");
}

void sighandler(int sig)
{
	if (sig == SIGSEGV) {
		printf("[I] received signal: SIGSEGV\n");
		exit(0);
	} else
		printf("Unexpected signal %d received.\n", sig);
}

void setup(void)
{
	sprintf(file1, "mprotect03.tmp.%d", getpid());
}

void cleanup(void)
{
}

