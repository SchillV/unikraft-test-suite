#include "incl.h"
void setup();
void cleanup();
char *TCID = "fcntl09";
int TST_TOTAL = 2;
char fname[255];
int fd;
struct flock flocks;
int main(int ac, char **av)
{
	setup();
	int type, ok=1;
	for (type = 0; type < 2; type++) {
		flocks.l_type = type ? F_RDLCK : F_WRLCK;
		long int ret = fcntl(fd, F_SETLK, &flocks);
		if (ret == -1) {
			printf("fcntl(%s, F_SETLK, &flocks) flocks.l_type = %s Failed, errno=%d\n", fname, type ? "F_RDLCK" : "F_WRLCK",errno);
			ok = 0;
		} else {
			printf( "fcntl(%s, F_SETLK, &flocks) flocks.l_type = %s returned %ld\n", fname, type ? "F_RDLCK" : "F_WRLCK", ret);
		}
		flocks.l_type = F_UNLCK;
		ret = fcntl(fd, F_SETLK, &flocks);
		if (ret == -1) {
			printf("fcntl(%s, F_SETLK, &flocks) flocks.l_type = F_UNLCK Failed, errno=%d\n", fname, errno);
			ok = 0;
		} else {
			printf("fcntl(%s, F_SETLK, &flocks) flocks.l_type = F_UNLCK returned %ld\n", fname, ret);
		}
	}
	if(ok)
		printf("test succeeded\n");
	cleanup();
}
void setup(void)
{
	sprintf(fname, "./file_%d", getpid());
	if ((fd = creat(fname, 0644)) == -1) {
		printf("creat(%s, 0644) Failed, errno=%d", fname, errno);
		cleanup();
		exit(0);
	} else if (close(fd) == -1) {
		printf("close(%s) Failed, errno=%d\n",fname, errno);
		cleanup();
		exit(0);
	} else if ((fd = open(fname, O_RDWR, 0700)) == -1) {
		printf("open(%s, O_RDWR,0700) Failed, errno=%d\n", fname, errno);
		cleanup();
		exit(0);
	}
	flocks.l_whence = 1;
	flocks.l_start = 0;
	flocks.l_len = 0;
	flocks.l_pid = getpid();
}
void cleanup(void)
{
	if (close(fd) == -1) {
		printf("close(%s) Failed, errno=%d\n", fname, errno);
	} else if (unlink(fname) == -1) {
		printf("unlink(%s) Failed, errno=%d\n", fname, errno);
	}
}

