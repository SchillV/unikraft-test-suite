#include "incl.h"
#define	INVAL_IOCTL	9999999

int fd, fd_file;

int bfd = -1;

struct termio termio;

struct tcase {
	int *fd;
	int request;
	struct termio *s_tio;
	int error;
} tcases[] = {
	{&bfd, TCGETA, &termio, EBADF},
	{&fd, TCGETA, (struct termio *)-1, EFAULT},
	 * by kernel commit 07d106d0 and bbb63c51
	 */
	{&fd, INVAL_IOCTL, &termio, ENOTTY},
	{&fd_file, TCGETA, &termio, ENOTTY},
	{&fd, TCGETA, NULL, EFAULT}
};

char *device;

int  verify_ioctl(unsigned int i)
{
	TST_EXP_FAIL(ioctl(*(tcases[i].fd), tcases[i].request, tcases[i].s_tio),
		     tcases[i].error);
}

void setup(void)
{
	if (!device)
		tst_brk(TBROK, "You must specify a tty device with -D option");
	fd = open(device, O_RDWR, 0777);
	fd_file = open("x", O_CREAT, 0777);
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	if (fd_file > 0)
		close(fd_file);
}

void main(){
	setup();
	cleanup();
}
