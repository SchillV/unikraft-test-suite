#include "incl.h"

void setup(void);

void cleanup(void);
char *TCID = "fcntl08";
int TST_TOTAL = 1;

int fd;
int main(int ac, char **av)
{
	setup();
	int ret = fcntl(fd, F_SETFL, O_NDELAY | O_APPEND | O_NONBLOCK);
	if (ret == -1) {
		printf("fcntl failed, error number %d\n", errno);
	} else {
		printf("fcntl returned %d\ntest succeeded\n",ret);
	}
	cleanup();
}
void setup(void)
{
	fd = open("test_file", O_RDWR | O_CREAT, 0700);
}
void cleanup(void)
{
	if (close(fd) == -1)
		printf("close failed\n");
}

