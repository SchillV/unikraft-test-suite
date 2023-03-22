#include "incl.h"
#define MNT_POINT "mntpoint"
#define OLD_FILE_NAME MNT_POINT"/oldfile"
#define NEW_FILE_NAME MNT_POINT"/newfile"
#define OLD_DIR_NAME MNT_POINT"/olddir"
#define NEW_DIR_NAME MNT_POINT"/newdir"

struct stat old_file_st, old_dir_st, new_file_st, new_dir_st;

void run(void)
{
	touch(OLD_FILE_NAME, 0700, NULL);
	mkdir(OLD_DIR_NAME, 00770);
	touch(NEW_FILE_NAME, 0700, NULL);
	mkdir(NEW_DIR_NAME, 00770);
	stat(OLD_FILE_NAME, &old_file_st);
	stat(OLD_DIR_NAME, &old_dir_st);
	TST_EXP_PASS(rename(OLD_FILE_NAME, NEW_FILE_NAME),
						"rename(%s, %s)",
						OLD_FILE_NAME, NEW_FILE_NAME);
	TST_EXP_PASS(rename(OLD_DIR_NAME, NEW_DIR_NAME),
						"rename(%s, %s)",
						OLD_DIR_NAME, NEW_DIR_NAME);
	stat(NEW_FILE_NAME, &new_file_st);
	stat(NEW_DIR_NAME, &new_dir_st);
	TST_EXP_EQ_LU(old_file_st.st_dev, new_file_st.st_dev);
	TST_EXP_EQ_LU(old_file_st.st_ino, new_file_st.st_ino);
	TST_EXP_EQ_LU(old_dir_st.st_dev, new_dir_st.st_dev);
	TST_EXP_EQ_LU(old_dir_st.st_ino, new_dir_st.st_ino);
	TST_EXP_FAIL(stat(OLD_FILE_NAME, &old_file_st),
				ENOENT,
				"stat(%s, &old_file_st)",
				OLD_FILE_NAME);
	TST_EXP_FAIL(stat(OLD_DIR_NAME, &old_dir_st),
				ENOENT,
				"stat(%s, &old_dir_st)",
				OLD_DIR_NAME);
	unlink(NEW_FILE_NAME);
	rmdir(NEW_DIR_NAME);
}

void main(){
	setup();
	cleanup();
}
