#include "incl.h"
void setup(void);
void cleanup(void);
char *TCID = "fcntl01";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int flags;
	char fname[40];
	int fd[10], fd2[10];
	int mypid;
	unsigned int i;
	int lc;
	setup();
	mypid = getpid();
	for (i = 0; i < 8; i++) {
		sprintf(fname, "./fcntl%d.%d", i, mypid);
		if ((fd[i] = open(fname, O_WRONLY | O_CREAT, 0666)) == -1){
			printf("open failed\n");
			return 0;
		}
		fd2[i] = fd[i];
	}
		close(fd[2]);
		close(fd[3]);
		close(fd[4]);
		close(fd[5]);
		if ((fd[2] = fcntl(fd[1], F_DUPFD, 1)) == -1){
			printf("fcntl(.., 1) failed, error number %d\n", errno);
			return 0;
		}if (fd[2] < fd2[2]){
			printf("new fd has unexpected value: got %d, expected greater than %d\n", fd[2], 5);
			return 0;
		}if ((fd[4] = fcntl(fd[1], F_DUPFD, fd2[3])) < 0){
			printf("fcntl(.., fd2[3]) failed, error number %d\n", errno);
			return 0;
		}if (fd[4] < fd2[3]){
			printf("new fd has unexpected value, got %d, expect greater than %d\n", fd[4], fd2[3]);
			return 0;
		}if ((fd[8] = fcntl(fd[1], F_DUPFD, fd2[5])) < 0){
			printf("fcntl(.., fd2[5]) failed, error number %d\n", errno);
			return 0;
		}if (fd[8] != fd2[5]){
			printf("new fd has unexpected value: got %d, expected %d\n", fd[8], fd2[5]);
			return 0;
		}
		flags = fcntl(fd[2], F_GETFL, 0);
		if ((flags & O_WRONLY) == 0){
			printf("unexpected flag 0x%x, expected 0x%x\n", flags, O_WRONLY);
			return 0;
		}if (fcntl(fd[2], F_SETFL, O_NDELAY) == -1){
			printf("fcntl(.., O_NDELAY) failed\n");
			return 0;
		}
		flags = fcntl(fd[2], F_GETFL, 0);
		if ((flags & (O_NDELAY | O_WRONLY)) == 0){
			printf("unexpected flag 0x%x, expected 0x%x\n",flags, O_NDELAY | O_WRONLY);
			return 0;
		}if (fcntl(fd[2], F_SETFL, O_APPEND) == -1){
			printf("fcntl(.., O_APPEND) failed, error number %d\n", errno);
			return 0;
		}
		flags = fcntl(fd[2], F_GETFL, 0);
		if ((flags & (O_APPEND | O_WRONLY)) == 0){
			printf("unexpected flag ox%x, expected 0x%x\n", flags, O_APPEND | O_WRONLY);
			return 0;
		}if (fcntl(fd[2], F_SETFL, O_NDELAY | O_APPEND) < 0){
			printf("fcntl(.., O_NDELAY|O_APPEND) failed\n");
			return 0;
		}
		flags = fcntl(fd[2], F_GETFL, 0);
		if ((flags & (O_NDELAY | O_APPEND | O_WRONLY)) == 0){
			printf("unexpected flag 0x%x, expected 0x%x\n", flags, O_NDELAY | O_APPEND | O_SYNC | O_WRONLY);
			return 0;
		}if (fcntl(fd[2], F_SETFL, 0) == -1){
			printf("fcntl(.., 0) failed\n");
			return 0;
		}
		flags = fcntl(fd[2], F_GETFL, 0);
		if ((flags & O_WRONLY) == 0){
			printf("unexpected flag 0x%x, expected 0x%x\n",flags, O_WRONLY);
			return 0;
		}if ((flags = fcntl(fd[2], F_GETFD, 0)) < 0){
			printf("fcntl(.., F_GETFD, ..) #1 failed, error number %d", errno);
			return 0;
		}if (flags != 0){
			printf("unexpected flags got 0x%x expected 0x%x\n", flags, 0);
			return 0;
		}if ((flags = fcntl(fd[2], F_SETFD, 1)) == -1){
			printf("fcntl(.., F_SETFD, ..) failed\n");
			return 0;
		}if ((flags = fcntl(fd[2], F_GETFD, 0)) == -1){
			printf("fcntl(.., F_GETFD, ..) #2 failed, error number %d\n", errno);
			return 0;
		}if (flags != 1){
			printf("unexpected flags, got 0x%x, expected 0x%x\n", flags, 1);
			return 0;
		}for (i = 0; i < sizeof(fd)/sizeof(fd[0]); i++)
			close(fd[i]);
		for (i = 0; i < 8; i++) {
			sprintf(fname, "./fcntl%u.%d", i, mypid);
			if ((unlink(fname)) == -1){
				printf("unlinking %s failed, error number %d\n", fname, errno);
				return 0;
			}
		}
	printf("test succeeded\n");
	cleanup();
}
void setup(void)
{
}
void cleanup(void)
{
}

