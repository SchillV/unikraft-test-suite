#include "incl.h"
#define TESTFILE        "test_file"
#define LINK_TESTFILE   "link_test_file"
#define FILE_SIZE       1024
#define FILE_MODE	0644
#define NLINK	        2

struct stat stat_buf;

uid_t user_id;

gid_t group_id;

int fildes;

void run(void)
{
	TST_EXP_PASS(fstat(fildes, &stat_buf));
	TST_EXP_EQ_LU(stat_buf.st_uid, user_id);
	TST_EXP_EQ_LU(stat_buf.st_gid, group_id);
	TST_EXP_EQ_LI(stat_buf.st_size, FILE_SIZE);
	TST_EXP_EQ_LU(stat_buf.st_mode & 0777, FILE_MODE);
	TST_EXP_EQ_LU(stat_buf.st_nlink, NLINK);
}

void setup(void)
{
	user_id  = getuid();
	group_id = getgid();
	umask(0);
	fildes = open(TESTFILE, O_WRONLY | O_CREAT, FILE_MODE);
	if (tst_fill_file(TESTFILE, 'a', FILE_SIZE, 1))
		tst_brk(TBROK, "Could not fill test file");
	link(TESTFILE, LINK_TESTFILE);
}

void cleanup(void)
{
	if (fildes > 0)
		close(fildes);
}

void main(){
	setup();
	cleanup();
}
