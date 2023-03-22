#include "incl.h"

const char pathname[] = "unlinkattestdir",
		  subpathname[] = "unlinkatsubtestdir",
		  subpathdir[] = "unlinkattestdir/unlinkatsubtestdir",
		  testfile[] = "unlinkattestfile.txt",
		  testfile2[] = "unlinkattestdir/unlinkattestfile.txt";

char *testfile3;

int fd;

int getfd(int i)
{
	if (i == 2)
		fd = open(testfile3, O_CREAT | O_RDWR, 0600);
	else
		fd = open(pathname, O_DIRECTORY);
	return fd;
}

struct tcase {
	int fd;
	const char *filename;
	int flag;
	int exp_errno;
} tc[] = {
	{0, testfile, 0, 0},
	{0, NULL, 0, 0},
	{0, testfile, 0, ENOTDIR},
	{100, testfile, 0, EBADF},
	{0, testfile, 9999, EINVAL},
	{AT_FDCWD, testfile, 0, 0},
	{0, subpathname, AT_REMOVEDIR, 0},
};

void run(unsigned int i)
{
	int fd3 = -1;
	if (access(testfile2, F_OK))
		fprintf(testfile2, testfile2);
	if (access(testfile3, F_OK))
		fd3 = open(testfile3, O_CREAT | O_RDWR, 0600);
	if (access(subpathdir, F_OK))
		mkdir(subpathdir, 0700);
	if (access(testfile, F_OK))
		fprintf(testfile, testfile);
	if (tc[i].fd)
unlinkat(tc[i].fd, tc[i].filename, tc[i].flag);
	else
unlinkat(getfd(i), tc[i].filename, tc[i].flag);
	if (TST_ERR == tc[i].exp_errno)
		tst_res(TPASS | TTERRNO, "unlinkat() returned expected errno");
	else
		tst_res(TFAIL | TTERRNO, "unlinkat() failed");
	if (!tc[i].fd)
		close(fd);
	if (fd3 > 0)
		close(fd3);
}

void setup(void)
{
	char buf[PATH_MAX];
	getcwd(buf, PATH_MAX);
	asprintf(&testfile3, "%s/unlinkatfile3.txt", buf);
	tc[1].filename = testfile3;
	mkdir(pathname, 0700);
}

void cleanup(void)
{
	unlink(testfile);
	unlink(testfile2);
	rmdir(pathname);
}

void main(){
	setup();
	cleanup();
}
