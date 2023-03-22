#include "incl.h"

const char *fname = "test_file";

const char palfa[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#define PALFA_LEN (sizeof(palfa)-1)

int  verify_read()
{
	int fd;
	char prbuf[BUFSIZ];
	fd = open(fname, O_RDONLY);
int ret = read(fd, prbuf, BUFSIZ);
	if (ret != PALFA_LEN) {
		printf("Bad read count - got %d - expected %zu\n",
				ret, PALFA_LEN);
		close(fd);
		return -1;
	}
	if (memcmp(palfa, prbuf, PALFA_LEN)) {
		printf("read buffer not equal to write buffer\n");
		close(fd);
		return -1;
	}
	printf("read() data correctly\n");
	close(fd);
	return 1;
}

void setup(void)
{
	int fd;
	fd = creat(fname, 0777);
	write(fd, palfa, PALFA_LEN);
	close(fd);
}

void main(){
	setup();
	if(verify_read() == 1)
		printf("test succeeded\n");
	else
		printf("test failed\n");
}
