#include "incl.h"

void sighandler(int sig);

void cleanup(void);

void setup(void);
char *TCID = "mprotect02";
int TST_TOTAL = 1;

int fd, status;

char file1[BUFSIZ];

char *addr = MAP_FAILED;

char buf[] = "abcdefghijklmnopqrstuvwxyz";
int main(int ac, char **av)
{
	int ok = 1;
	int bytes_to_write, fd;
	size_t num_bytes;
	pid_t pid;
	setup();
	fd = open(file1, O_RDWR | O_CREAT, 0777);
	num_bytes = getpagesize();
	do {
		bytes_to_write = strlen(buf)< num_bytes ? strlen(buf) : num_bytes;
		num_bytes -= write(fd, buf, bytes_to_write);
	} while (0 < num_bytes);
	addr = mmap(0, sizeof(buf), PROT_READ, MAP_SHARED, fd, 0);
	if ((pid = fork()) == -1){
		printf("fork #1 failed, error number %d\n", errno);
		cleanup();
		exit(0);
	}
	if (pid == 0) {
		memcpy(addr, buf, strlen(buf));
		exit(255);
	}
	waitpid(pid, &status, 0);
	if (!WIFEXITED(status)){
		printf("child exited abnormally with status: %d\n", status);
		cleanup();
		exit(0);
	}
	switch (status) {
	case 255:
		printf("memcpy did not generate SIGSEGV\n");
		cleanup();
		ok = 0;
	case 0:
		printf("got SIGSEGV as expected\n");
		break;
	default:
		printf("got unexpected signal: %d\n", status);
		cleanup();
		ok = 0;
		break;
	}
	int ret = mprotect(addr, sizeof(buf), PROT_WRITE);
	if (ret != -1) {
		if ((pid = fork()) == -1){
			printf("fork #2 failed\n");
			cleanup();
			ok = 0;
		} if (pid == 0) {
			memcpy(addr, buf, strlen(buf));
			exit(0);
		}
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) &&
			WEXITSTATUS(status) == 0)
			printf("didn't get SIGSEGV\n");
		else{
			printf("child exited abnormally\n");
			ok = 0;
		}
	} else {
		printf("mprotect failed, error number %d\n", errno);
		ok = 0;
	}
	munmap(addr, sizeof(buf));
	addr = MAP_FAILED;
	close(fd);
	unlink(file1);
	cleanup();
	if(ok)
		printf("test succeeded\n");
}

void sighandler(int sig)
{
	_exit((sig == SIGSEGV) ? 0 : sig);
}

void setup(void)
{;
	sprintf(file1, "mprotect02.tmp.%d", getpid());
}

void cleanup(void)
{
	if (addr != MAP_FAILED) {
		munmap(addr, sizeof(buf));
		close(fd);
	}
}

