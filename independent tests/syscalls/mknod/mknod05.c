#include "incl.h"
#define LTPUSER		"nobody"
#define MODE_RWX	S_IFIFO | S_IRWXU | S_IRWXG | S_IRWXO
#define MODE_SGID       S_IFIFO | S_ISGID | S_IRWXU | S_IRWXG | S_IRWXO
#define DIR_TEMP	"testdir_5"
#define TNODE		"tnode_%d"
char *TCID = "mknod05";
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	int fflag;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 *  Attempt to create a filesystem node with group id (sgid)
		 *  bit set on a directory with group id (sgid) bit set
		 *  such that, the node created by mknod(2) should have
		 *  group id (sgid) bit set and node's gid should be equal
		 *  to that of effective gid of the process.
		 */
mknod(node_name, MODE_SGID, 0);
		if (TEST_RETURN == -1) {
			tst_resm(TFAIL, "mknod(%s, %#o, 0)  failed, errno=%d : "
				 "%s", node_name, MODE_SGID, TEST_ERRNO,
				 strerror(TEST_ERRNO));
			continue;
		}
		fflag = 1;
		if (stat(node_name, &buf) < 0) {
			tst_resm(TFAIL, "stat() of %s failed, errno:%d",
				 node_name, TEST_ERRNO);
			fflag = 0;
		}
		if (!(buf.st_mode & S_ISGID)) {
			tst_resm(TFAIL, "%s: Incorrect modes, "
				 "setgid bit not set", node_name);
			fflag = 0;
		}
		if (buf.st_gid != group2_gid) {
			tst_resm(TFAIL, "%s: Incorrect group",
				 node_name);
			fflag = 0;
		}
		if (fflag) {
			tst_resm(TPASS, "Functionality of mknod(%s, "
				 "%#o, 0) successful",
				 node_name, MODE_SGID);
		}
		if (unlink(node_name) == -1) {
			tst_resm(TWARN, "unlink(%s) failed, errno:%d %s",
				 node_name, errno, strerror(errno));
		}
	}
	chdir(cleanup, "..");
	 * Invoke cleanup() to delete the test directories created
	 * in the setup() and exit main().
	 */
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_require_root();
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	save_myuid = getuid();
	mypid = getpid();
	sprintf(node_name, TNODE, mypid);
	if ((user1 = getpwnam(LTPUSER)) == NULL) {
		tst_brkm(TBROK, cleanup, "%s not in /etc/passwd", LTPUSER);
	}
	user1_uid = user1->pw_uid;
	group1_gid = user1->pw_gid;
	group2_gid = getegid();
	 * Create a test directory under temporary directory with the
	 * specified mode permissions, with uid/gid set to that of guest
	 * user and the test process.
	 */
	mkdir(cleanup, DIR_TEMP, MODE_RWX);
	chown(cleanup, DIR_TEMP, user1_uid, group2_gid);
	chmod(cleanup, DIR_TEMP, MODE_SGID);
	 * Verify that test directory created with expected permission modes
	 * and ownerships.
	 */
	stat(cleanup, DIR_TEMP, &buf);
	if (!(buf.st_mode & S_ISGID)) {
		tst_brkm(TBROK, cleanup,
			 "%s: Incorrect modes, setgid bit not set", DIR_TEMP);
	}
	if (buf.st_gid != group2_gid) {
		tst_brkm(TBROK, cleanup, "%s: Incorrect group", DIR_TEMP);
	}
	chdir(cleanup, DIR_TEMP);
}
void cleanup(void)
{
	tst_rmdir();
}

