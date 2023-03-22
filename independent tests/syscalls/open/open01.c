#include "incl.h"
#define TEST_FILE	"testfile"
#define TEST_DIR	"testdir"

int fd;

struct tcase {
	char *filename;
	int flag;
	mode_t mode;
	unsigned short tst_bit;
	char *desc;
} tcases[] = {
	{TEST_FILE, O_RDWR | O_CREAT, 01444, S_ISVTX, "sticky bit"},
	{TEST_DIR, O_DIRECTORY, 0, S_IFDIR, "sirectory bit"}
};

int  verify_open(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct stat buf;
	TST_EXP_FD_SILENT(open(tc->filename, tc->flag, tc->mode),
	           "open() with %s", tc->desc);
	if (!TST_PASS)
		return;
	fd = TST_RET;
	fstat(fd, &buf);
	if (!(buf.st_mode & tc->tst_bit))
		tst_res(TFAIL, "%s is cleared unexpectedly", tc->desc);
	else
		tst_res(TPASS, "%s is set as expected", tc->desc);
	close(fd);
	if (S_ISREG(buf.st_mode))
		unlink(tc->filename);
}

void setup(void)
{
	mkdir(TEST_DIR, 0755);
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
