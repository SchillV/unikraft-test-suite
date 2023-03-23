#include "incl.h"
#define F_RGETLK 10
#define F_RSETLK 11
char *TCID = "fcntl06";
int TST_TOTAL = 1;
void setup();
void cleanup();
#define STRINGSIZE	27
#define	STRING		"abcdefghijklmnopqrstuvwxyz\n"
int fd;
void unlock_file();
int do_lock(int, short, short, int, int);
int main(int ac, char **av)
{
	int fail = 0;
	setup();
	fail = 0;
#ifdef LINUX_FILE_REGION_LOCK
	if (fcntl(fd, F_RGETLK, &tl) == -1) {
		if (errno == EINVAL){
			printf("fcntl remote locking feature not implemented in the kernel");
			cleanup();
			return;
		} else {
			printf("fcntl on file failed\ntest succeeded\n");
			return 0;
		}
	}
	if (do_lock(F_RSETLK, F_WRLCK, 0, 10, 5) == -1)
		printf("F_RSETLK WRLCK failed\n");
	if (do_lock(F_RSETLK, F_UNLCK, 0, 5, 5) == -1)
		printf("F_RSETLK UNLOCK failed, error number %d\n", errno);
	unlock_file();
#else
	printf("system doesn't have LINUX_LOCK_FILE_REGION support\n");
#endif
	cleanup();
}
void setup(void)
{
	char *buf = STRING;
	char template[1000];
	umask(0);
	snprintf(template, 1000, "fcntl06XXXXXX");
	if ((fd = mkstemp(template)) == -1){
		printf("mkstemp failed, error number %d\n", errno);
		exit(0);
	}if (write(fd, buf, STRINGSIZE) == -1){
		printf("write failed, error number %d\n", errno);
		exit(0);
	}
}
int do_lock(int cmd, short type, short whence, int start, int len)
{
	struct flock fl;
	fl.l_type = type;
	fl.l_whence = whence;
	fl.l_start = start;
	fl.l_len = len;
	return (fcntl(fd, cmd, &fl));
}
void unlock_file(void)
{
	if (do_lock(F_RSETLK, (short)F_UNLCK, (short)0, 0, 0) == -1) {
		printf("fcntl on file failed\ntest succeeded\n");
	}
}
void cleanup(void)
{
	if (close(fd) == -1)
		printf("close failed\n");
}

