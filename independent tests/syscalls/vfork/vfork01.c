#include "incl.h"
#define _GNU_SOURCE 1
char *TCID = "vfork01";
int TST_TOTAL = 1;
uid_t Peuid, Ceuid, Csuid, Psuid, Pruid, Cruid;
gid_t Pegid, Cegid, Psgid, Csgid, Prgid, Crgid;
mode_t Pumask, Cumask;
				 * pathname of working directory of
				 * child/parent process.
				 */
struct stat StatPbuf;
struct stat StatCbuf;
struct stat Stat_cwd_Pbuf;
struct stat Stat_cwd_Cbuf;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Call vfork(2) to create a child process without
		 * fully copying the address space of parent.
		 */
vfork();
		if ((cpid = TEST_RETURN) == -1) {
			tst_resm(TFAIL, "vfork() Failed, errno=%d : %s",
				 TEST_ERRNO, strerror(TEST_ERRNO));
			 * Get the euid, ruid, egid, rgid, umask value
			 * and the current working directory of the
			 * child process
			 */
			if (getresuid(&Cruid, &Ceuid, &Csuid) < 0) {
				tst_resm(TFAIL, "getresuid() fails to "
					 "get real/eff./saved uid of "
					 "child process");
				_exit(1);
			}
			if (getresgid(&Crgid, &Cegid, &Csgid) < 0) {
				tst_resm(TFAIL, "getresgid() fails to "
					 "get real/eff./saved gid of "
					 "child process");
				_exit(1);
			}
			 * Get the file mode creation mask value of
			 * child process by setting value zero and
			 * restore the previous mask value.
			 */
			Cumask = umask(0);
			 * Restore the process mask of child to
			 * previous value.
			 */
			umask(Cumask);
			 * Get the pathname of current working
			 * directory for the child process.
			 */
			if ((Ccwd = (char *)getcwd(NULL,
						   BUFSIZ)) == NULL) {
				tst_resm(TFAIL, "getcwd failed for the "
					 "child process");
				_exit(1);
			}
			 * Get the device number and the inode
			 * number of "/" directory for the child
			 * process.
			 */
			if (stat("/", &StatCbuf) < 0) {
				tst_resm(TFAIL, "stat(2) failed to get "
					 "info. of'/' in the child "
					 "process");
				_exit(1);
			}
			 * Get the device/inode number of "."
			 * (working directory) for the child process.
			 */
			if (stat(Ccwd, &Stat_cwd_Cbuf) < 0) {
				tst_resm(TFAIL, "stat(2) failed to get "
					 "info. of working irectory in "
					 "the child");
				_exit(1);
			}
			if (Peuid != Ceuid || Pegid != Cegid ||
			    Psuid != Csuid || Psgid != Csgid ||
			    Pruid != Cruid || Prgid != Crgid ||
			    Pumask != Cumask) {
				tst_resm(TFAIL, "Attribute values of "
					 "parent and child don't match");
				_exit(1);
			} else {
				tst_resm(TINFO, "Attribute values of "
					 "parent and child match");
			}
			if (strcmp(Pcwd, Ccwd) != 0) {
				tst_resm(TFAIL, "Working directories "
					 "of parent and child don't "
					 "match");
				_exit(1);
			} else {
				tst_resm(TINFO, "Working directories "
					 "of parent and child match");
			}
			 * Check for the same device/inode number of
			 * '/' directory.
			 */
			if ((StatPbuf.st_ino != StatCbuf.st_ino) ||
			    (StatPbuf.st_dev != StatCbuf.st_dev)) {
				tst_resm(TFAIL, "Device/inode number "
					 "of parent and childs '/' "
					 " don't match");
				_exit(1);
			} else {
				tst_resm(TINFO, "Device/inode number "
					 "of parent and childs '/' "
					 "match");
			}
			 * Check for the same device and inode number
			 *  of "." (current working directory.
			 */
			if ((Stat_cwd_Pbuf.st_ino !=
			     Stat_cwd_Cbuf.st_ino) ||
			    (Stat_cwd_Pbuf.st_dev !=
			     Stat_cwd_Cbuf.st_dev)) {
				tst_resm(TFAIL, "Device/inode number "
					 "of parent and childs '.' "
					 "don't match");
				_exit(1);
			} else {
				tst_resm(TINFO, "Device/inode number "
					 "of parent and childs '.' "
					 "don't match");
			}
			 * Exit with normal exit code if everything
			 * fine
			 */
			_exit(0);
			 * Let the parent process wait till child completes
			 * its execution.
			 */
			wait(&exit_status);
			if (WEXITSTATUS(exit_status) == 0) {
				tst_resm(TPASS, "Call of vfork() successful");
			} else if (WEXITSTATUS(exit_status) == 1) {
				tst_resm(TFAIL,
					 "Child process exited abnormally");
			}
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	 * Get the euid, ruid, egid, rgid, umask value
	 * and the current working directory of the parent process.
	 */
	if (getresuid(&Pruid, &Peuid, &Psuid) < 0) {
		tst_brkm(TFAIL, cleanup, "getresuid() fails to get "
			 "real/eff./saved uid of parent");
	}
	if (getresgid(&Prgid, &Pegid, &Psgid) < 0) {
		tst_brkm(TFAIL, cleanup, "getresgid() fails to get "
			 "real/eff./saved gid of parent");
	}
	Pumask = umask(0);
				 * Restore the mask value of the
				 * process.
				 */
	 * Get the pathname of current working directory of the parent
	 * process.
	 */
	if ((Pcwd = (char *)getcwd(NULL, BUFSIZ)) == NULL) {
		tst_brkm(TFAIL, cleanup,
			 "getcwd failed for the parent process");
	}
	 * Get the device and inode number of root directory for the
	 * parent process.
	 */
	if (stat("/", &StatPbuf) == -1) {
		tst_brkm(TFAIL, cleanup, "stat(2) failed to get info. of '/' "
			 "in parent process");
	}
	 * Get the device number and the inode number of "." (current-
	 * working directory) for the parent process.
	 */
	if (stat(Pcwd, &Stat_cwd_Pbuf) < 0) {
		tst_brkm(TFAIL, cleanup, "stat(2) failed to get info. of "
			 "working directory in parent process");
	}
}
void cleanup(void)
{
}

