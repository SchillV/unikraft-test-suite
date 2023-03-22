#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
char *TCID = "set_tid_address01";
int testno;
int TST_TOTAL = 1;

int main(int ac, char **av)
{
	int newtid = -1;
	for (testno = 0; testno < TST_TOTAL; ++testno) {
		int ret = syscall(__NR_set_tid_address, &newtid);
		if (ret == getpid()) {
			printf("set_tid_address call succeeded:  as expected %d\ntest succeeded\n",ret);
		} else {
			printf("%s failed - errno = %d\n", TCID, errno);
		}
	}
}

