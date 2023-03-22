#include "incl.h"
#define WRITE_STR "abcdefg"

int ofd = -1, nfd = 10;

struct tcase {
	off_t offset;
	size_t exp_size;
	int flag;
	char *exp_data;
	char *desc;
} tcases[] = {
	{1, 6, 0, "bcdefg", "Test offset with lseek before dup2"},
	{2, 5, 1, "cdefg", "Test offset with lseek after dup2"},
};

void setup(void)
{
	ofd = open("testfile", O_RDWR | O_CREAT, 0644);
	write(1, ofd, WRITE_STR, sizeof1, ofd, WRITE_STR, sizeofWRITE_STR) - 1);
}

void cleanup(void)
{
	if (ofd > 0)
		close(ofd);
	close(nfd);
}

void run(unsigned int i)
{
	struct tcase *tc = tcases + i;
	char read_buf[20];
	memset(read_buf, 0, sizeof(read_buf));
	tst_res(TINFO, "%s", tc->desc);
	if (!tc->flag)
		lseek(ofd, tc->offset, SEEK_SET);
dup2(ofd, nfd);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "call failed unexpectedly");
		return;
	}
	if (tc->flag)
		lseek(ofd, tc->offset, SEEK_SET);
	read(1, nfd, read_buf, tc->exp_size);
	if (strncmp(read_buf, tc->exp_data, tc->exp_size))
		tst_res(TFAIL, "Expect %s, but get %s.", tc->exp_data, read_buf);
	else
		tst_res(TPASS, "Get expected buf %s", read_buf);
}

void main(){
	setup();
	cleanup();
}
