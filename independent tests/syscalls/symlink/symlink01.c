#include "incl.h"
void setup(void);
void cleanup(void);
void help(void);
void delete_files(char *path1, char *path2);
struct all_test_cases;
void do_EEXIST(struct all_test_cases *tc_ptr);
void do_ENOENT(struct all_test_cases *tc_ptr);
void do_ELOOP(struct all_test_cases *tc_ptr);
void do_ENOTDIR(struct all_test_cases *tc_ptr);
void do_EXDEV(struct all_test_cases *tc_ptr);
void do_ENAMETOOLONG(struct all_test_cases *tc_ptr);
void do_EINVAL(struct all_test_cases *tc_ptr);
void do_readlink(struct all_test_cases *tc_ptr);
void do_stat(struct all_test_cases *tc_ptr);
void do_chdir(struct all_test_cases *tc_ptr);
void do_link(struct all_test_cases *tc_ptr);
void do_unlink(struct all_test_cases *tc_ptr);
void do_chmod(struct all_test_cases *tc_ptr);
void do_utime(struct all_test_cases *tc_ptr);
void do_rename(struct all_test_cases *tc_ptr);
void do_open(struct all_test_cases *tc_ptr);
struct tcses;
int do_syscalltests(struct tcses *tcs);
struct tcses *get_tcs_info(char *ptr);
#define BIG_STRING "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
#define DEFAULT_TCID  "symlink01"
#define SYMLINK "symlink01"
#define READLINK "readlink01"
#define STAT "stat04"
#define STAT_64 "stat04_64"
#define LSTAT "lstat01"
#define LSTAT_64 "lstat01_64"
#define MKDIR "mkdir05"
#define RMDIR "rmdir03"
#define CHDIR "chdir01"
#define LINK "link01"
#define UNLINK "unlink01"
#define CHMOD "chmod01"
#define UTIME "utime01"
#define RENAME "rename01"
#define OPEN "open01"
#define cktcsid(s1,s2) (!strcmp(s1,s2))
#define BUFMAX 512
#define MODE 0700
const char *msgs[] = {
	"Creation of symbolic link file to no object file is ok",
	"Creation of symbolic link file and object file via symbolic link is ok",
	"Creating an existing symbolic link file error is caught",
	"Creating a symbolic link which exceeds maximum pathname error is caught",
	"Reading of symbolic link file contents checks out ok",
	"Reading a symbolic link which exceeds maximum pathname error is caught",
	"Getting stat info about object file through symbolic link file is ok",
	"Stat(2) error when accessing non-existent object through symbolic link is caught",
	"lstat(2) of symbolic link file which points to no object file is ok",
	"lstat(2) of symbolic link file which points at an object file is ok",
	"mkdir(2) of object file through symbolic link file failed as expected",
	"rmdir(2) of object file through symbolic link file failed as expected",
	"chdir(2) to object file location through symbolic link file is ok",
	"chdir(2) to non-existent object file location through symbolic link file failed as expected",
	"link(2) to a symbolic link, which is pointing to an existing object file worked - file created and link count adjusted",
	"link(2) to a symbolic link, which is pointing to a non-existing object file worked ok - file created and link count adjusted.",
	"unlink(2) of symbolic link file with no object file removal is ok",
	"chmod(2) of object file permissions through symbolic link file is ok",
	"chmod(2) error when accessing non-existent object through symbolic link is caught",
	"utime(2) change of object file access and modify times through symbolic link file is ok",
	"utime(2) error when accessing non-existent object through symbolic link is caught",
	"rename(3) of symbolic link file name which points at no object file is ok",
	"rename(3) of symbolic link file name which points at object file is ok",
	"rename(3) error of symbolic link file name across file systems is caught",
	"open(2) with (O_CREAT | O_RDWR) to create object file through symbolic link file and all writes, reads, and lseeks are ok",
	"open(2) with O_RDWR of existing  object file through symbolic link file and all writes, reads, and lseeks are ok",
	"open(2) with (O_CREAT | O_EXCL) error  is caught when creating object file through symbolic link file",
	"open(2) error with O_RDWR is caught when processing symbolic link file which points at no object file",
	"Nested symbolic link access condition caught.  ELOOP is returned",
	"Reading a nonsymbolic link file error condition is caught.  EINVAL is returned",
	"lstat(2) of object file returns object file inode information",
	"NULL"
};
int creat_both(char *path1, char *path2, char *path3);
int creat_symlink(char *path1, char *path2, char *_path3);
int creat_path_max(char *path1, char *path2, char *path3);
int ck_symlink(char *path1, char *path2, char *path3);
int creat_object(char *path1, char *_path2, char *_path3);
int ck_object(char *path1, char *path2, char *path3);
int ck_both(char *path1, char *path2, char *path3);
int ck_path_max(char *path1, char *path2, char *path3);
struct all_test_cases {
	char *tcid;
	int test_fail;
	int errno_val;
	int pass_msg;
	int (*test_setup) (char *path1, char *path2, char *path3);
	int (*ck_test) (char *path1, char *path2, char *path3);
	char *fn_arg[3];
} test_objects[] = {
	{
		SYMLINK, 0, 0, 0, creat_symlink, ck_symlink, {
	"%bc+eFhi!k", S_FILE, NULL}}, {
		SYMLINK, 0, 0, 0, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		SYMLINK, 0, 0, 1, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}}, {
		SYMLINK, 1, EEXIST, 2, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		SYMLINK, 1, ENAMETOOLONG, 3, creat_path_max, ck_path_max, {
	O_FILE, S_FILE, NULL}}, {
		READLINK, 0, 0, 4, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		READLINK, 0, 0, 4, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}}, {
		READLINK, 1, ENAMETOOLONG, 5, creat_path_max, ck_path_max, {
	O_FILE, S_FILE, NULL}}, {
		READLINK, 1, EINVAL, 29, creat_object, ck_object, {
	O_FILE, NULL, NULL}}, {
		STAT, 0, 0, 6, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}},
	{
		STAT, 1, ENOENT, 7, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		STAT, 1, ELOOP, 28, creat_symlink, ck_symlink, {
	S_FILE, S_FILE, NULL}}, {
		STAT_64, 0, 0, 6, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}}, {
		STAT_64, 1, ENOENT, 7, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		STAT_64, 1, ELOOP, 28, creat_symlink, ck_symlink, {
	S_FILE, S_FILE, NULL}}, {
		LSTAT, 0, 0, 8, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		LSTAT, 0, 0, 9, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}}, {
		LSTAT, 0, 0, 30, creat_object, ck_object, {
	O_FILE, NULL, NULL}}, {
		LSTAT_64, 0, 0, 8, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		LSTAT_64, 0, 0, 9, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}},
	{
		LSTAT_64, 0, 0, 30, creat_object, ck_object, {
	O_FILE, NULL, NULL}}, {
		MKDIR, 1, EEXIST, 10, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		RMDIR, 1, ENOTDIR, 11, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		CHDIR, 0, 0, 12, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, O_FILE}}, {
		CHDIR, 1, ENOENT, 13, creat_symlink, ck_symlink, {
	"%bc+eFhi!k", S_FILE, NULL}}, {
		CHDIR, 1, ELOOP, 28, creat_symlink, ck_symlink, {
	S_FILE, S_FILE, NULL}}, {
		LINK, 0, 0, 14, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}}, {
		LINK, 0, 0, 15, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}},
	{
		LINK, 1, -1, -1, creat_symlink, ck_symlink, {
	NULL, NULL, NULL}}, {
		UNLINK, 0, 0, 16, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}},
	{
		CHMOD, 0, 0, 17, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}}, {
		CHMOD, 1, ENOENT, 18, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		CHMOD, 1, ELOOP, 28, creat_symlink, ck_symlink, {
	S_FILE, S_FILE, NULL}}, {
		UTIME, 0, 0, 19, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}}, {
		UTIME, 1, ENOENT, 20, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		UTIME, 1, ELOOP, 28, creat_symlink, ck_symlink, {
	S_FILE, S_FILE, NULL}}, {
		RENAME, 0, 0, 21, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		RENAME, 0, 0, 22, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}},
	{
		RENAME, 1, EXDEV, 23, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}}, {
		OPEN, 0, 0, 24, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}},
	{
		OPEN, 0, 0, 25, creat_both, ck_both, {
	O_FILE, S_FILE, O_FILE}}, {
		OPEN, 1, EEXIST, 26, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, O_FILE}}, {
		OPEN, 1, ENOENT, 27, creat_symlink, ck_symlink, {
	O_FILE, S_FILE, NULL}}, {
		OPEN, 1, ELOOP, 28, creat_symlink, ck_symlink, {
	S_FILE, S_FILE, NULL}}
};
struct tcses {
	char *tcid;
	char *syscall;
	struct all_test_cases *tc_ptr;
	char *desc;
} all_tcses[] = {
	{
	SYMLINK, "symlink", 5, &test_objects[0],
		    "Make a Symbolic Link to a File"}, {
	READLINK, "readlink", 4, &test_objects[5],
		    "Reads Value of a Symbolic Link"}, {
	STAT, "stat", 3, &test_objects[9],
		    "Gets File Status Indirectly From a Symbolic Link file"}, {
	STAT_64, "stat64", 3, &test_objects[12],
		    "Gets File Status Indirectly From a Symbolic Link file"}, {
	LSTAT, "lstat", 3, &test_objects[15],
		    "Get file Status About a Symbolic Link File"}, {
	LSTAT_64, "lstat64", 3, &test_objects[18],
		    "Get file Status About a Symbolic Link File"}, {
	MKDIR, "mkdir", 1, &test_objects[21],
		    "Fail When Making a Directory File Indirectly from a symlink"},
	{
	RMDIR, "rmdir", 1, &test_objects[22],
		    "Fail When Removing a Directory File Indirectly from a symlink"},
	{
	CHDIR, "chdir", 3, &test_objects[23],
		    "Changes CWD Location Indirectly from a symlink"}, {
	LINK, "link", 2, &test_objects[26],
		    "Creates a Link To a File Indirectly From a Symbolic"}, {
	UNLINK, "unlink", 1, &test_objects[29],
		    "Removes a Link To a File but not the Object File"}, {
	CHMOD, "chmod", 3, &test_objects[30],
		    "Change Object File Permissions Indirectly From a Symbolic"},
	{
	UTIME, "utime", 3, &test_objects[33],
		    "Set File Access And Modify Object File Times via symlink"},
	{
	RENAME, "rename", 2, &test_objects[36],
		    "Rename a Symbolic Link File And Not Any Object file"}, {
OPEN, "open", 5, &test_objects[39],
		    "Create/Open a File For Reading Or Writing via symlink"},};
int TST_TOTAL;
int TEST_RESULT;
time_t a_time_value = 100;
char *TCID;
char test_msg[BUFMAX];
struct stat asymlink, statter;
char Buffer[1024];
char Buf[1024];
char *Tcid = NULL;
option_t Options[] = {
	{NULL, NULL, NULL}
};
int main(int argc, char *argv[])
{
	struct tcses *tcs_ptr;
	int lc;
	tst_parse_opts(argc, argv, Options, &help);
	 * If the -T option was used, use that TCID or use the default
	 */
	if (Tcid != NULL) {
		TCID = Tcid;
		Selectedtests = Tcid;
	}
#ifndef ALL
	else {
		TCID = DEFAULT_TCID;
		Selectedtests = DEFAULT_TCID;
	}
#endif
	 * Get test case specification information and assign TST_TOTAL
	 */
	if ((tcs_ptr = get_tcs_info(Selectedtests)) == NULL) {
		TST_TOTAL = 1;
		tst_brkm(TBROK, cleanup,
			 "Unknown symbolic link test case specification executed");
	}
    * perform global setup for test
    ***************************************************************/
	setup();
     * check looping state if -c option given
     ***************************************************************/
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Execute tcs testing function and all defined test cases
		 */
		do_syscalltests(tcs_ptr);
	}
	 * End appropriately
	 */
	cleanup();
	tst_exit();
}
struct tcses *get_tcs_info(char *ptr)
{
	int ctr;
	struct tcses *tcs_ptr;
#if ALL
	if (ptr == NULL) {
		TST_TOTAL = 0;
		for (ctr = 1; ctr < sizeof(all_tcses) / sizeof(struct tcses);
		     ctr++)
			TST_TOTAL += all_tcses[ctr].test_cases;
		return all_tcses;
	}
#endif
	for (ctr = 0; ctr < (int)(sizeof(all_tcses) / sizeof(struct tcses)); ctr++) {
		if (strcmp(ptr, all_tcses[ctr].tcid) == 0 ||
		    strcmp(ptr, all_tcses[ctr].syscall) == 0) {
			tcs_ptr = &all_tcses[ctr];
			TCID = all_tcses[ctr].tcid;
			TST_TOTAL = tcs_ptr->test_cases;
			return (tcs_ptr);
		}
	}
	return NULL;
}

int see_if_a_symlink(char *path)
{
	if (lstat(path, &asymlink) < 0)
		return (-1);
	if ((asymlink.st_mode & S_IFMT) == S_IFLNK)
		return 1;
	else
		return 0;
}
void delete_files(char *path1, char *path2)
{
	unlink(path1);
	unlink(path2);
}
int creat_symlink(char *path1, char *path2, char *_path3)
{
symlink(path1, path2);
	errno = TEST_ERRNO;
	if (TEST_RETURN == -1) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"symlink(2) Failure when creating setup %s object file: errno:%d %s",
			path1, errno, strerror(errno));
		return 0;
	} else {
		sprintf(Buf, "symlink(%s, %s) was succesful.\n", path1, path2);
		strcat(Buffer, Buf);
#if DEBUG
		tst_resm(TPASS, "symlink(%s, %s) was succesful.", path1, path2);
#endif
	}
	return 1;
}
#define creat_symlink(p1, p2) creat_symlink(p1, p2, NULL)
int creat_object(char *path1, char *_path2, char *_path3)
{
	int fd;
	if ((fd = creat(path1, MODE)) == -1) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"creat(2) Failure when creating setup %s object file: errno:%d %s",
			path1, errno, strerror(errno));
		return 0;
	} else {
		sprintf(Buf, "creat(%s, %#o) was succesful.\n", path1, MODE);
		strcat(Buffer, Buf);
#if DEBUG
		tst_resm(TPASS, "creat(%s, %#o) was succesful.", path1, MODE);
#endif
	}
	if (close(fd) == -1) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"close(2) Failure when closing setup %s object file: errno:%d %s",
			path1, errno, strerror(errno));
		return 0;
	}
	return 1;
}
#define creat_object(p1) creat_object(p1, NULL, NULL)
int creat_both(char *path1, char *path2, char *path3)
{
	if (creat_symlink(path1, path2) == -1)
		return 0;
	else if (creat_object(path3) == -1)
		return 0;
	return 1;
}
int ck_symlink(char *path1, char *path2, char *path3)
{
	int ret;
	if ((ret = see_if_a_symlink(path2)) == -1) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"lstat(2) Failure when accessing %s symbolic link file which should contain %s path to %s file ",
			path2, path1, path3);
		return 0;
	} else if (ret == 0) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"%s is not a symbolic link file which contains %s path to %s file",
			path2, path1, path3);
		return 0;
	}
	return 1;
}
int ck_both(char *path1, char *path2, char *path3)
{
	if (ck_symlink(path1, path2, path3) == 0)
		return 0;
	else if ((stat(path3, &statter) == -1) && (errno == ENOENT)) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"stat(2) Failure when accessing %s object file ",
			path3);
		return 0;
	} else if ((stat(path2, &asymlink) == -1) && (errno == ENOENT)) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"stat(2) Failure when accessing %s symbolic link file ",
			path2);
		return 0;
	} else if (statter.st_ino != asymlink.st_ino) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"stat(2) Failure when accessing %s object file through %s symbolic link file ",
			path3, path2);
		return 0;
	}
	return 1;
}
int creat_path_max(char *path1, char *path2, char *path3)
{
	int ctr, to_go, size, whole_chunks;
	char *cwd;
	if ((cwd = getcwd(NULL, 0)) == NULL) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"getcwd(3) Failure in setup of %s %s %s test case object elements",
			path1, path2, path3);
		return 0;
	}
	cwd = getcwd(NULL, 0);
	size = strlen(cwd);
	to_go = PATH_MAX - size;
	size = strlen(path1);
	whole_chunks = to_go / size;
	strcpy(full_path, cwd);
	for (ctr = 0; ctr < whole_chunks; ctr++) {
		strcat(full_path, path1);
	}
	size = strlen(full_path);
	to_go = PATH_MAX - size;
	strcat(full_path, "/");
	for (ctr = 0; ctr < to_go; ctr++)
		strcat(full_path, "Z");
	return 1;
}
int ck_path_max(char *path1, char *path2, char *path3)
{
	if (strlen(full_path) == (PATH_MAX + 1))
		return 1;
	else {
		TEST_RESULT = TBROK;
		sprintf(test_msg, "%s %d %s %s %s %s",
			"full_path character array length was not",
			(PATH_MAX + 1),
			"characters long for test case object elements", path1,
			path2, path3);
		return 0;
	}
}
int ck_object(char *path1, char *path2, char *path3)
{
	int ret;
	if ((ret = see_if_a_symlink(path1)) < 0) {
		TEST_RESULT = TFAIL;
		sprintf(test_msg,
			"lstat(2) failed to return inode information for a regular object file");
		return 0;
	} else if (ret == 1) {
		TEST_RESULT = TFAIL;
		sprintf(test_msg,
			"lstat(2) detected a regular object file as a symbolic link file");
		return 0;
	} else if (stat(path1, &statter) == -1) {
		TEST_RESULT = TBROK;
		sprintf(test_msg,
			"stat(2) failed to return inode information for a regular object file");
		return 0;
	} else if (memcmp((char *)&statter, (char *)&asymlink, sizeof(statter))
		   != 0) {
		TEST_RESULT = TFAIL;
		sprintf(test_msg,
			"lstat(2) and stat(2) do not return same inode information for an object file");
		return 0;
	}
	return 1;
}
int do_syscalltests(struct tcses *tcs)
{
	int ctr, ret;
	struct all_test_cases *tc_ptr;
	 * loop through desired number of test cases
	 */
	for (ctr = 0, tc_ptr = tcs->tc_ptr; ctr < TST_TOTAL; ctr++, tc_ptr++) {
		Buffer[0] = '\0';
		 * If running all test cases for all tcid, set the TCID if needed.
		 */
		if (Selectedtests == NULL) {
			if (strcmp(tcs->tcid, tc_ptr->tcid) != 0) {
				TCID = tc_ptr->tcid;
				tst_count = 0;
			}
		}
		 * Insure that we are executing the correct tcs test case
		 */
		if (strcmp(tcs->tcid, tc_ptr->tcid) != 0) {
			tst_resm(TBROK,
				 "%s TCID attempted to execute %s %d %d test case",
				 tcs->tcid, tc_ptr->tcid, tc_ptr->test_fail,
				 tc_ptr->errno_val);
			continue;
		}
		TEST_RESULT = TPASS;
		delete_files(S_FILE, O_FILE);
		 * Perform test case setup
		 */
		ret =
		    (tc_ptr->test_setup) (tc_ptr->fn_arg[0], tc_ptr->fn_arg[1],
					  tc_ptr->fn_arg[2]);
		if (tc_ptr->test_fail) {
			 * Try to perform test verification function
			 */
			if (!(tc_ptr->ck_test)
			    (tc_ptr->fn_arg[0], tc_ptr->fn_arg[1],
			     tc_ptr->fn_arg[2]))
				tst_resm(TEST_RESULT, "%s", test_msg);
			else if (tc_ptr->errno_val == EEXIST)
				do_EEXIST(tc_ptr);
			else if (tc_ptr->errno_val == ENOENT)
				do_ENOENT(tc_ptr);
			else if (tc_ptr->errno_val == ELOOP)
				do_ELOOP(tc_ptr);
			else if (tc_ptr->errno_val == ENOTDIR)
				do_ENOTDIR(tc_ptr);
			else if (tc_ptr->errno_val == EXDEV)
				do_EXDEV(tc_ptr);
			else if (tc_ptr->errno_val == ENAMETOOLONG)
				do_ENAMETOOLONG(tc_ptr);
			else if (tc_ptr->errno_val == EINVAL)
				do_EINVAL(tc_ptr);
			else
				tst_resm(TBROK, "Test Case Declaration Error");
			if (tc_ptr->errno_val != 0)
				tst_resm(TBROK, "Test Case Declaration Error");
			else {
				 * Perform test verification function
				 */
				ret =
				    (tc_ptr->ck_test) (tc_ptr->fn_arg[0],
						       tc_ptr->fn_arg[1],
						       tc_ptr->fn_arg[2]);
				if ((cktcsid(tc_ptr->tcid, SYMLINK)) ||
				    (cktcsid(tc_ptr->tcid, LSTAT)) ||
				    (cktcsid(tc_ptr->tcid, LSTAT_64))) {
					if (ret == 1)
						tst_resm(TEST_RESULT, "%s",
							 msgs[tc_ptr->
							      pass_msg]);
					else
						tst_resm(TEST_RESULT, "%s",
							 test_msg);
				} else if (ret == 0)
					tst_resm(TEST_RESULT, "%s", test_msg);
				else if (cktcsid(tc_ptr->tcid, READLINK))
					do_readlink(tc_ptr);
				else if (cktcsid(tc_ptr->tcid, STAT))
					do_stat(tc_ptr);
				else if (cktcsid(tc_ptr->tcid, STAT_64))
					do_stat(tc_ptr);
				else if (cktcsid(tc_ptr->tcid, CHDIR))
					do_chdir(tc_ptr);
				else if (cktcsid(tc_ptr->tcid, LINK))
					do_link(tc_ptr);
				else if (cktcsid(tc_ptr->tcid, UNLINK))
					do_unlink(tc_ptr);
				else if (cktcsid(tc_ptr->tcid, CHMOD))
					do_chmod(tc_ptr);
				else if (cktcsid(tc_ptr->tcid, UTIME))
					do_utime(tc_ptr);
				else if (cktcsid(tc_ptr->tcid, RENAME))
					do_rename(tc_ptr);
				else if (cktcsid(tc_ptr->tcid, OPEN))
					do_open(tc_ptr);
				else
					tst_resm(TBROK,
						 "Unknown test case processing actions declared");
			}
		} else
			tst_resm(TBROK, "Test Case Declaration Error");
	}
	return 0;
}
void do_EEXIST(struct all_test_cases *tc_ptr)
{
	if (cktcsid(tc_ptr->tcid, SYMLINK)) {
symlink(tc_ptr->fn_arg[0], tc_ptr->fn_arg[1]);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == EEXIST))
			tst_resm(TPASS, "%s", msgs[tc_ptr->pass_msg]);
		else
			tst_resm(TFAIL, "%s %s",
				 "Expected EEXIST error when creating a symbolic link file",
				 "which already existed");
	} else if (cktcsid(tc_ptr->tcid, MKDIR)) {
mkdir(tc_ptr->fn_arg[1], MODE);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == EEXIST)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s %s",
				 "Expected EEXIST error when creating a directory by a symbolic",
				 "link file which pointed at no object file");
			rmdir(tc_ptr->fn_arg[1]);
		}
	} else if (cktcsid(tc_ptr->tcid, OPEN)) {
open(tc_ptr->fn_arg[1], (O_EXCL | O_CREAT), 0666);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == EEXIST)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s %s errno:%d %s",
				 "Expected EEXIST error for exclusively opening an object file",
				 "through a symbolic link file was not received:",
				 errno, strerror(errno));
		}
	} else
		tst_resm(TBROK,
			 "Unknown test case processing actions declared");
}
void do_ENOENT(struct all_test_cases *tc_ptr)
{
	if ((cktcsid(tc_ptr->tcid, STAT)) || (cktcsid(tc_ptr->tcid, STAT_64))) {
		if ((stat(tc_ptr->fn_arg[1], &asymlink) == -1)
		    && (errno == ENOENT)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s %s errno:%d %s",
				 "Expected ENOENT error for stating a non-existent directory",
				 "through a symbolic link file was not received:",
				 errno, strerror(errno));
		}
	} else if (cktcsid(tc_ptr->tcid, CHDIR)) {
		if ((chdir(tc_ptr->fn_arg[1]) == -1) && (errno == ENOENT)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s %s errno:%d %s",
				 "Expected ENOENT error for changing to a non-existent",
				 "directory through a symbolic link file was not received:",
				 errno, strerror(errno));
			chdir(tst_get_tmpdir());
		}
	} else if (cktcsid(tc_ptr->tcid, LINK)) {
		if ((link(tc_ptr->fn_arg[1], "nick") == -1)
		    && (errno == ENOENT)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s %s errno:%d %s",
				 "Expected ENOENT error condition when link(2) a symbolic",
				 "link which pointed at no object:", errno,
				 strerror(errno));
			delete_files("nick", NULL);
		}
	} else if (cktcsid(tc_ptr->tcid, CHMOD)) {
		if ((chmod(tc_ptr->fn_arg[1], MODE) == -1) && (errno == ENOENT)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s %s errno:%d %s",
				 "Expected ENOENT error condition when chmod(2) a symbolic",
				 "link which pointed at no object,", errno,
				 strerror(errno));
		}
	} else if (cktcsid(tc_ptr->tcid, UTIME)) {
		if ((utime(tc_ptr->fn_arg[1], NULL) == -1) && (errno == ENOENT)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s %s errno:%d %s",
				 "Expected ENOENT error condition when utime(2) a symbolic",
				 "link which pointed at no object:", errno,
				 strerror(errno));
		}
	} else if (cktcsid(tc_ptr->tcid, OPEN)) {
		if ((open(tc_ptr->fn_arg[1], O_RDWR) == -1)
		    && (errno == ENOENT)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s %s errno:%d %s",
				 "Expected ENOENT error for opening a non-existent object",
				 " file through a symbolic link file was not received,",
				 errno, strerror(errno));
		}
	} else
		tst_resm(TBROK,
			 "Unknown test case processing actions declared");
}
void do_ELOOP(struct all_test_cases *tc_ptr)
{
	if ((cktcsid(tc_ptr->tcid, STAT)) || (cktcsid(tc_ptr->tcid, STAT_64))) {
stat(tc_ptr->fn_arg[1], &asymlink);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == ELOOP)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TEST_RESULT, "%s errno:%d %s",
				 "Expected ELOOP errno from stat(2) (nested symb link),",
				 errno, strerror(errno));
		}
	} else if (cktcsid(tc_ptr->tcid, CHDIR)) {
chdir(tc_ptr->fn_arg[1]);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == ELOOP)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s errno:%d %s",
				 "Expected ELOOP error condition when chdir(2) a nested symbolic link:",
				 errno, strerror(errno));
			chdir(tst_get_tmpdir());
		}
	} else if (cktcsid(tc_ptr->tcid, LINK)) {
link(tc_ptr->fn_arg[1], O_FILE);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == ELOOP)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s errno:%d %s",
				 "Expected ELOOP error condition when link(2) a nested symbolic link:",
				 errno, strerror(errno));
		}
	} else if (cktcsid(tc_ptr->tcid, CHMOD)) {
chmod(tc_ptr->fn_arg[1], MODE);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == ELOOP)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s errno:%d %s",
				 "Expected ELOOP error condition when chmod(2) a nested symbolic link:",
				 errno, strerror(errno));
		}
		return;
	} else if (cktcsid(tc_ptr->tcid, UTIME)) {
utime(tc_ptr->fn_arg[1], NULL);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == ELOOP)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s errno:%d %s",
				 "Expected ELOOP error condition when utime(2) a nested symbolic link:",
				 errno, strerror(errno));
		}
	} else if (cktcsid(tc_ptr->tcid, OPEN)) {
		int fd;
open(tc_ptr->fn_arg[1], O_CREAT, 0666);
		fd = TEST_RETURN;
		errno = TEST_ERRNO;
		if ((fd == -1) && (errno == ELOOP)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s errno:%d %s",
				 "Expected ELOOP error condition when open(2) a nested symbolic link:",
				 errno, strerror(errno));
		}
	} else
		tst_resm(TBROK,
			 "Unknown test case processing actions declared");
}
void do_ENOTDIR(struct all_test_cases *tc_ptr)
{
	if (cktcsid(tc_ptr->tcid, RMDIR)) {
mkdir(tc_ptr->fn_arg[0], MODE);
		errno = TEST_ERRNO;
		if (TEST_RETURN == -1)
			tst_resm(TBROK, "mkdir(2) Failure when creating %s",
				 tc_ptr->fn_arg[0]);
		else if ((rmdir(tc_ptr->fn_arg[1]) == -1) && (errno == ENOTDIR)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
			rmdir(tc_ptr->fn_arg[0]);
		} else {
			tst_resm(TFAIL, "%s %s errno:%d %s",
				 "Expected ENOTDIR error for removing a non-existent",
				 "directory through a symbolic link file was not received,",
				 errno, strerror(errno));
		}
	} else
		tst_resm(TBROK,
			 "Unknown test case processing actions declared");
}
void do_EXDEV(struct all_test_cases *tc_ptr)
{
	if (cktcsid(tc_ptr->tcid, RENAME)) {
rename(tc_ptr->fn_arg[1], Y_A_S_FILE);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == EXDEV)) {
			if (see_if_a_symlink(Y_A_S_FILE) == -1) {
				tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
			} else {
				tst_resm(TFAIL,
					 "%s %s %s file outside of current file system",
					 "rename(3) returned -1 when trying to move symbolic link file",
					 "outside of current file system, but created",
					 Y_A_S_FILE);
			}
		} else {
			tst_resm(TFAIL, "%s %s errno:%d %s",
				 "Expected EXDEV error for renaming an existing symbolic",
				 "link file to a location outside of existing file system,",
				 errno, strerror(errno));
			delete_files("/NiCkEr", NULL);
		}
	} else
		tst_resm(TBROK,
			 "Unknown test case processing actions declared");
}
void do_ENAMETOOLONG(struct all_test_cases *tc_ptr)
{
	int ret;
	if (cktcsid(tc_ptr->tcid, SYMLINK)) {
symlink(tc_ptr->fn_arg[0], full_path);
		errno = TEST_ERRNO;
		if ((TEST_RETURN == -1) && (errno == ENAMETOOLONG)) {
			if (see_if_a_symlink(full_path) == -1) {
				tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
			} else {
				tst_resm(TFAIL, "%s %s %d %s",
					 "symlink(2) returned -1 when trying to create a symbolic",
					 "link file whose name exceeded",
					 (PATH_MAX + 1),
					 "characters, but it created the symbolic link file");
			}
		} else {
			tst_resm(TFAIL | TERRNO,
				 "Expected ENAMETOOLONG error when creating %s symbolic link file with a path exceeding %d characters",
				 tc_ptr->fn_arg[1], (PATH_MAX + 1));
		}
	} else if (cktcsid(tc_ptr->tcid, READLINK)) {
		char scratch[PATH_MAX + 1];
		ret = readlink(full_path, scratch, strlen(full_path));
		if ((ret == -1) && (errno == ENAMETOOLONG)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL,
				 "Expected ENAMETOOLONG error when reading %s symbolic link file with a path exceeding %d characters: errno:%d %s",
				 tc_ptr->fn_arg[1], (PATH_MAX + 1), errno,
				 strerror(errno));
		}
	} else
		tst_resm(TBROK,
			 "Unknown test case processing actions declared");
}
void do_EINVAL(struct all_test_cases *tc_ptr)
{
	if (cktcsid(tc_ptr->tcid, READLINK)) {
readlink(tc_ptr->fn_arg[0], test_msg, BUFMAX);
		errno = TEST_ERRNO;
		if (TEST_RETURN == -1) {
			if (errno == EINVAL) {
				tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
			} else {
				tst_resm(TFAIL,
					 "readlink(2) ret:-1, errno:%d, : Exp errno:%d",
					 errno, EINVAL);
			}
		} else {
			tst_resm(TFAIL,
				 "readlink(2) did not returned -1 when reading %s",
				 "a file which is not a symbolic link file");
		}
	} else
		tst_resm(TBROK,
			 "Unknown test case processing actions declared");
}
void do_readlink(struct all_test_cases *tc_ptr)
{
	char scratch[PATH_MAX];
	int ret;
	ret = readlink(tc_ptr->fn_arg[1], scratch, strlen(tc_ptr->fn_arg[0]));
	if (ret == -1) {
		tst_resm(TFAIL, "readlink(2) failure on %s symbolic link file",
			 tc_ptr->fn_arg[1]);
	} else
	    if (strncmp(tc_ptr->fn_arg[0], scratch, strlen(tc_ptr->fn_arg[0]))
		!= 0) {
		scratch[strlen(tc_ptr->fn_arg[0])] = '\0';
		tst_resm(TFAIL,
			 "readlink(2) Error : Expected %s symbolic link file contents but %s actual contents were returned",
			 tc_ptr->fn_arg[0], scratch);
	} else {
		tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
	}
}
void do_stat(struct all_test_cases *tc_ptr)
{
	if (statter.st_dev != asymlink.st_dev)
		tst_resm(TFAIL,
			 "stat of symbolic link reference to object device info %jd != stat of object file device info %jd",
			 (intmax_t) statter.st_dev, (intmax_t) asymlink.st_dev);
	else if (statter.st_mode != asymlink.st_mode)
		tst_resm(TFAIL,
			 "stat of symbolic link reference to object file permissions %jd != stat of object file permissions %jd",
			 (intmax_t) statter.st_mode,
			 (intmax_t) asymlink.st_mode);
	else if (statter.st_nlink != asymlink.st_nlink)
		tst_resm(TFAIL,
			 "stat of symbolic link reference to object file link count %jd != stat of object file link count %jd",
			 (intmax_t) statter.st_nlink,
			 (intmax_t) asymlink.st_nlink);
	else if (statter.st_uid != asymlink.st_uid)
		tst_resm(TFAIL,
			 "stat of symbolic link reference to object file uid %jd != stat of object file uid %jd",
			 (intmax_t) statter.st_uid, (intmax_t) asymlink.st_uid);
	else if (statter.st_gid != asymlink.st_gid)
		tst_resm(TFAIL,
			 "stat of symbolic link reference to object file gid %jd != stat of object file gid %jd",
			 (intmax_t) statter.st_gid, (intmax_t) asymlink.st_gid);
	else if (statter.st_size != asymlink.st_size)
		tst_resm(TFAIL,
			 "stat of symbolic link reference to object file size %ld != stat of object file size %ld",
			 statter.st_size, asymlink.st_size);
	else if (statter.st_atime != asymlink.st_atime)
		tst_resm(TFAIL,
			 "stat of symbolic link reference to object access time %ld != stat of object file access time %ld",
			 statter.st_atime, asymlink.st_atime);
	else if (statter.st_mtime != asymlink.st_mtime)
		tst_resm(TFAIL,
			 "stat of symbolic link reference to object modify time %ld != stat of object file modify time %ld",
			 statter.st_atime, asymlink.st_atime);
	else if (statter.st_ctime != asymlink.st_ctime)
		tst_resm(TFAIL,
			 "stat of symbolic link reference to object change time %ld != stat of object file change time %ld",
			 statter.st_atime, asymlink.st_atime);
	else
		tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
}
void do_chdir(struct all_test_cases *tc_ptr)
{
	if (mkdir(tc_ptr->fn_arg[2], MODE) == -1)
		tst_resm(TFAIL, "Could not create a setup directory file");
	else {
		sprintf(Buf, "mkdir(%s, %#o) was successful\n",
			tc_ptr->fn_arg[2], MODE);
		strcat(Buffer, Buf);
		if (chdir(tc_ptr->fn_arg[1]) == -1)
			tst_resm(TFAIL,
				 "%sCould not change a directory file through a %s",
				 Buffer,
				 "symbolic link which which pointed at object");
		else {
			char *cwd;
			char expected_location[PATH_MAX];
			 *  Build expected current directory position
			 */
			strcpy(expected_location, tst_get_tmpdir());
			strcat(expected_location, "/");
			strcat(expected_location, tc_ptr->fn_arg[2]);
			if ((cwd = getcwd(NULL, 0)) == NULL) {
				tst_resm(TFAIL, "getcwd(3) FAILURE");
			} else if (strcmp(cwd, expected_location) == 0) {
				tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
			} else {
				tst_resm(TFAIL,
					 "%s%s %s %s not equal to expected %s",
					 Buffer,
					 "chdir(2) returned successfully, but getcwd(3) indicated",
					 "new current working directory location",
					 cwd, expected_location);
			}
			chdir(tst_get_tmpdir());
		}
		rmdir(tc_ptr->fn_arg[2]);
	}
}
void do_link(struct all_test_cases *tc_ptr)
{
	struct stat stbuf;
	if (link(tc_ptr->fn_arg[1], "nick") == -1) {
		tst_resm(TFAIL, "%slink(%s, \"nick\") failed, errno: %d: %s %s",
			 Buffer, tc_ptr->fn_arg[1], errno,
			 "link of new file to object file via symbolic link file failed",
			 "when expected not to");
	} else {
		sprintf(Buf, "link(%s, \"nick\") was successful\n",
			tc_ptr->fn_arg[1]);
		strcat(Buffer, Buf);
		 * Check that links counts were properly set
		 */
		if (lstat(tc_ptr->fn_arg[1], &asymlink) == -1) {
			tst_resm(TBROK, "lstat(%s) failed. errno: %d",
				 tc_ptr->fn_arg[1], errno);
		} else if (lstat("nick", &statter) == -1) {
			tst_resm(TBROK, "lstat(nick) failed, errno:%d",
				 errno);
		} else {
			if (statter.st_ino == asymlink.st_ino) {
				if ((statter.st_nlink == 2) && (asymlink.st_nlink == 2)) {
					tst_resm(TEST_RESULT, "%s",
					         msgs[tc_ptr->pass_msg]);
				} else {
					lstat(tc_ptr->fn_arg[2],
					      &stbuf);
					tst_resm(TFAIL,
						 "%slink(%s, %s) failed to adjust link count.\n\
		count for nick is %d, count for %s is %d, count for %s is %d.",
						 Buffer, tc_ptr->fn_arg[1], "nick", statter.st_nlink, tc_ptr->fn_arg[1], asymlink.st_nlink, tc_ptr->fn_arg[2],
						 stbuf.st_nlink);
				}
			} else {
				tst_resm(TFAIL, "%sA lstat of %s (ino:%jd) and of\n\t\t\
%s (ino:%jd), does not show them being the same ino.", Buffer,
					 tc_ptr->fn_arg[1], (intmax_t) asymlink.st_ino, "nick", (intmax_t) statter.st_ino);
			}
		}
		delete_files("nick", NULL);
	}
}
void do_unlink(struct all_test_cases *tc_ptr)
{
	if (stat(tc_ptr->fn_arg[2], &asymlink) == -1)
		tst_resm(TBROK, "stat(2) Failure when accessing %s object file",
			 tc_ptr->fn_arg[2]);
	else if (unlink(tc_ptr->fn_arg[1]) == -1)
		tst_resm(TFAIL,
			 "unlink(2) failed when removing symbolic link file");
	else {
		sprintf(Buf, "unlink(%s) was successful\n", tc_ptr->fn_arg[1]);
		strcat(Buffer, Buf);
		if (stat(tc_ptr->fn_arg[2], &statter) == -1) {
			tst_resm(TFAIL, "%s %s",
				 "unlink(2) failed because it not only removed symbolic link",
				 "file which pointed at object file, but object file as well");
		} else if ((statter.st_ino == asymlink.st_ino) &&
			 (statter.st_dev == asymlink.st_dev) &&
			 (statter.st_size == asymlink.st_size)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s%s %s %s", Buffer,
				 "unlink(2) failed because it not only removed symbolic link",
				 "file which pointed at object file, but it changed object",
				 "file inode information");
		}
	}
}
void do_chmod(struct all_test_cases *tc_ptr)
{
	if (stat(tc_ptr->fn_arg[2], &asymlink) == -1)
		tst_resm(TBROK, "stat(2) Failure when accessing %s object file",
			 tc_ptr->fn_arg[2]);
	else if (chmod(tc_ptr->fn_arg[1], (MODE | MASK)) == -1)
		tst_resm(TFAIL, "%s%s %s", Buffer,
			 "chmod(2) failed when changing file permission",
			 "through symbolic link file");
	else {
		sprintf(Buf, "chmod(%s, %#o) was successful\n",
			tc_ptr->fn_arg[1], (MODE | MASK));
		strcat(Buffer, Buf);
		if (stat(tc_ptr->fn_arg[2], &statter) == -1) {
			tst_resm(TBROK,
				 "stat(2) Failure when accessing %s object file",
				 tc_ptr->fn_arg[2]);
		} else if ((statter.st_mode == (MODE | MASK))
			 && (statter.st_mode != asymlink.st_mode)) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s%s %o to %o %s", Buffer,
				 "chmod(2) failed to change object file permissions from",
				 asymlink.st_mode, (MODE | MASK),
				 "through symbolic link file");
		}
	}
}
void do_utime(struct all_test_cases *tc_ptr)
{
	struct utimbuf utimes;
	if (stat(tc_ptr->fn_arg[2], &asymlink) == -1)
		tst_resm(TBROK, "stat(2) Failure when accessing %s object file",
			 tc_ptr->fn_arg[2]);
	else {
		utimes.actime = asymlink.st_atime + a_time_value;
		utimes.modtime = asymlink.st_mtime + a_time_value;
		if (utime(tc_ptr->fn_arg[1], &utimes) == -1)
			tst_resm(TFAIL, "%s %s",
				 "utime(2) failed to process object file access and modify",
				 "time updates through symbolic link");
		else {
			if (stat(tc_ptr->fn_arg[2], &statter) == -1)
				tst_resm(TBROK,
					 "stat(2) Failure when accessing %s object file",
					 tc_ptr->fn_arg[2]);
			else {
				time_t temp, diff;
				temp = statter.st_atime - asymlink.st_atime;
				diff =
				    (statter.st_mtime - asymlink.st_mtime) -
				    temp;
				if (!diff) {
					tst_resm(TEST_RESULT, "%s",
					         msgs[tc_ptr->pass_msg]);
				} else {
					tst_resm(TFAIL,
						 "%s %s %jd greater than original times",
						 "utime(2) failed to change object file access and",
						 "modify times through symbolic link to a value",
						 (intmax_t) a_time_value);
				}
			}
		}
	}
}
void do_rename(struct all_test_cases *tc_ptr)
{
	int pts_at_object = 0;
	if (stat(tc_ptr->fn_arg[2], &statter) != -1)
		pts_at_object = 1;
rename(tc_ptr->fn_arg[1], A_S_FILE);
	errno = TEST_ERRNO;
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL,
			 "rename(3) failed to rename %s symbolic link file to %s",
			 tc_ptr->fn_arg[2], A_S_FILE);
	} else if (pts_at_object) {
		if (ck_both(tc_ptr->fn_arg[0], A_S_FILE, tc_ptr->fn_arg[2])) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
		} else {
			tst_resm(TFAIL, "%s", test_msg);
		}
	} else if (!ck_symlink(tc_ptr->fn_arg[0], A_S_FILE, NULL)) {
		tst_resm(TFAIL, "%s", test_msg);
	} else if (stat(tc_ptr->fn_arg[1], &asymlink) != -1) {
		tst_resm(TFAIL,
			 "rename(3) did not remove %s when renaming to %s",
			 tc_ptr->fn_arg[1], A_S_FILE);
	} else {
		tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
	}
}
void do_open(struct all_test_cases *tc_ptr)
{
	int fd = -1;
	int ret, pts_at_object = 0;
	char scratch[PATH_MAX];
	if (stat(tc_ptr->fn_arg[2], &statter) != -1)
		pts_at_object = 1;
	if (pts_at_object) {
open(tc_ptr->fn_arg[1], O_RDWR);
		errno = TEST_ERRNO;
		if ((fd = TEST_RETURN) == -1) {
			tst_resm(TFAIL,
				 "open(2) Failure when opening object file through symbolic link file");
			return;
		}
	} else {
open(tc_ptr->fn_arg[1], (O_CREAT | O_RDWR), MODE);
		errno = TEST_ERRNO;
		if ((fd = TEST_RETURN) == -1) {
			tst_resm(TFAIL,
				 "open(2) Failure when creating object file through symbolic link file");
			return;
		}
	}
	if ((ret = write(fd, BIG_STRING, strlen(BIG_STRING))) == -1) {
		tst_resm(TFAIL,
			 "write(2) Failure to object file opened through a symbolic link file: errno:%d",
			 errno);
	} else if (ret != strlen(BIG_STRING)) {
		tst_resm(TFAIL,
			 "write(2) Failed to write %zu bytes to object file opened through a symbolic link file",
			 strlen(BIG_STRING));
	} else if (lseek(fd, 0L, 0) == -1) {
		tst_resm(TFAIL,
			 "lseek(2) Failed to position to beginning of object file opened through a symbolic link file: errno = %d",
			 errno);
	} else if ((ret = read(fd, scratch, strlen(BIG_STRING))) == -1) {
		tst_resm(TFAIL,
			 "read(2) Failure of object file opened through a symbolic link file: errno = %d",
			 errno);
	} else if (ret != strlen(BIG_STRING)) {
		tst_resm(TFAIL,
			 "read(2) Failed to read %zu bytes to object file opened through a symbolic link file",
			 strlen(BIG_STRING));
	} else if (strncmp(BIG_STRING, scratch, strlen(BIG_STRING)) != 0) {
		tst_resm(TFAIL,
			 "Content of write(2) and read(2) Failed to object file through symbolic link file was not as expected. Expected %s and read returned %s",
			 BIG_STRING, scratch);
	} else {
		 *  Close off symbolic link file to object file access
		 */
		if (close(fd) == -1) {
			tst_resm(TFAIL,
				 "close(2) Failure when closing object file accessed symbolic link file");
		}
		 * Now, lets open up and read object file and compare contents
		 */
		else if ((fd = open(tc_ptr->fn_arg[0], O_RDONLY)) == -1) {
			tst_resm(TFAIL,
				 "open(2) Failure when opening %s file: errno:%d %s",
				 tc_ptr->fn_arg[0], errno, strerror(errno));
		} else if ((ret = read(fd, scratch, strlen(BIG_STRING))) == -1) {
			tst_resm(TFAIL,
				 "read(2) Failure of object file opened through a symbolic link file: errno:%d",
				 errno);
		} else if (ret != strlen(BIG_STRING)) {
			tst_resm(TFAIL,
				 "read(2) Failed to read %zu bytes to object file opened through a symbolic link file",
				 strlen(BIG_STRING));
		} else if (strncmp(BIG_STRING, scratch, strlen(BIG_STRING)) !=
			   0) {
			tst_resm(TFAIL,
				 "Content of write(2) and read(2) Failed to object file through symbolic link file was not as expected. Expected %s and read returned %s",
				 BIG_STRING, scratch);
		} else if (pts_at_object) {
			tst_resm(TEST_RESULT, "%s", msgs[tc_ptr->pass_msg]);
			if (ck_both
			    (tc_ptr->fn_arg[0], tc_ptr->fn_arg[1],
			     tc_ptr->fn_arg[0])) {
				tst_resm(TEST_RESULT, "%s",
					 msgs[tc_ptr->pass_msg]);
			} else {
				tst_resm(TFAIL, "%s", test_msg);
			}
		}
		close(fd);
	}
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
}
void cleanup(void)
{
	tst_rmdir();
}
void help(void)
{
	unsigned int ind;
	printf("   -T id  Determines which tests cases to execute:\n");
	for (ind = 0; ind < sizeof(all_tcses) / sizeof(struct tcses); ind++) {
		printf("  %s/%s - %s\n", all_tcses[ind].tcid,
		       all_tcses[ind].syscall, all_tcses[ind].desc);
	}
}

