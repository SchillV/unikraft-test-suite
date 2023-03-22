#include "incl.h"

char filename[40];

int fd;

void setup(void)
{
	sprintf(filename, "creat01.%d", getpid());
}

struct tcase {
	int mode;
} tcases[] = {
	{0644},
	{0444}
};

int  verify_creat(unsigned int i)
{
	struct stat buf;
	char c;
	fd = creat(filename, tcases[i].mode);
	stat(filename, &buf);
	if (buf.st_size != 0)
		tst_res(TFAIL, "creat() failed to truncate file to 0 bytes");
	else
		tst_res(TPASS, "creat() truncated file to 0 bytes");
	if (write(fd, "A", 1) != 1)
		tst_res(TFAIL | TERRNO, "write was unsuccessful");
	else
		tst_res(TPASS, "file was created and written to successfully");
	if (read(fd, &c, 1) != -1)
		tst_res(TFAIL, "read succeeded unexpectedly");
	else
		tst_res(TPASS | TERRNO, "read failed expectedly");
	close(fd);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

void main(){
	setup();
	cleanup();
}
