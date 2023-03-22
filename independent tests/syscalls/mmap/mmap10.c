#include "incl.h"
#define SIZE (5*1024*1024)
#define PATH_KSM "/sys/kernel/mm/ksm/"
char *TCID = "mmap10";
int TST_TOTAL = 1;

int fd, opt_anon, opt_ksm;

long ps;

char *x;
void setup(void);
void cleanup(void);
int mmapzero(int a);

int main(int argc, char *argv[])
{
	setup();
	printf("Starting tests\n");
	if(mmapzero(1) == 1 && mmapzero(2) == 1)
		printf("test succeeded\n");
	cleanup();
}

int mmapzero(int a)
{
	int n;
	if (a == 1) {
		x = mmap(NULL, SIZE + SIZE - ps, PROT_READ | PROT_WRITE,
			 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	} else {
		if ((fd = open("/dev/zero", O_RDWR, 0666)) < 0){
			printf("open failed, error number %d\n", errno);
			cleanup();
		}
		x = mmap(NULL, SIZE + SIZE - ps, PROT_READ | PROT_WRITE,
			 MAP_PRIVATE, fd, 0);
	}
	if (x == MAP_FAILED){
		printf("mmap failed, error number %d\n", errno);
		cleanup();
		return 0;
	}
#ifdef HAVE_DECL_MADV_MERGEABLE
	if (opt_ksm) {
		if (madvise(x, SIZE + SIZE - ps, MADV_MERGEABLE) == -1){
			printf("madvise failed, error number %d\n", errno);
			cleanup();
			return 0;
		}
	}
#endif
	x[SIZE] = 0;
	switch (n = fork()) {
	case -1:
		printf("fork failed, error number %d\n", errno);
		cleanup();
		return 0;
	case 0:
		if (munmap(x + SIZE + ps, SIZE - ps - ps) == -1){
			printf("munmap failed, erorr number %d\n", errno);
			cleanup();
			return 0;
		}
		exit(0);
	default:
		break;
	}
	switch (n = fork()) {
	case -1:
		printf("fork failed, error number %d\n", errno);
		cleanup();
		return 0;
	case 0:
		if (munmap(x + SIZE + ps, SIZE - ps - ps) == -1){
			printf("subsequent munmap #1, error number %d\n", errno);
			cleanup();
		}
		exit(0);
	default:
		switch (n = fork()) {
		case -1:
			printf("fork failed, error number %d\n", errno);
		case 0:
			if (munmap(x + SIZE + ps, SIZE - ps - ps) == -1){
				printf("subsequent munmap #2, error number %d\n", errno);
				cleanup();
			}
			exit(0);
		default:
			break;
		}
		break;
	}
	if (munmap(x, SIZE + SIZE - ps) == -1){
		printf("munmap all failed, erorr number %d\n", errno);
		cleanup();
		return 0;
	}
	while (waitpid(-1, &n, WUNTRACED | WCONTINUED) > 0)
		if (WEXITSTATUS(n) != 0){
			printf("child exit status is %d", WEXITSTATUS(n));
			return 0;
		}
	return 1;
}
void cleanup(void)
{
}
void setup(void)
{
	if ((ps = sysconf(_SC_PAGESIZE)) == -1){
		printf("sysconf(_SC_PAGESIZE)\n");
		cleanup();
		exit(EXIT_FAILURE);
	}
}

