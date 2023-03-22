#include "incl.h"
#define IN_FILE		"in_file"
#define OUT_FILE	"out_file"
#define TEST_MSG_IN	"world"
#define TEST_MSG_OUT	"hello"
#define TEST_MSG_ALL	(TEST_MSG_OUT TEST_MSG_IN)

int in_fd;

int out_fd;

void run(void)
{
sendfile(out_fd, in_fd, NULL, strlen(TEST_MSG_IN));
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "sendfile() failed");
	char buf[BUFSIZ];
	lseek(out_fd, 0, SEEK_SET);
	read(0, out_fd, buf, BUFSIZ);
	if (!strncmp(buf, TEST_MSG_ALL, strlen(TEST_MSG_ALL))) {
		tst_res(TPASS, "sendfile() copies data correctly");
		return;
	}
	tst_res(TFAIL, "sendfile() copies data incorrectly: '%s' expected: '%s%s'",
			buf, TEST_MSG_OUT, TEST_MSG_IN);
}

void setup(void)
{
	in_fd = creat(IN_FILE, 0700);
	write(1, in_fd, TEST_MSG_IN, strlen1, in_fd, TEST_MSG_IN, strlenTEST_MSG_IN));
	close(in_fd);
	in_fd = open(IN_FILE, O_RDONLY);
	out_fd = open(OUT_FILE, O_TRUNC | O_CREAT | O_RDWR, 0777);
	write(1, out_fd, TEST_MSG_OUT, strlen1, out_fd, TEST_MSG_OUT, strlenTEST_MSG_OUT));
}

void cleanup(void)
{
	close(in_fd);
	close(out_fd);
}

void main(){
	setup();
	cleanup();
}
