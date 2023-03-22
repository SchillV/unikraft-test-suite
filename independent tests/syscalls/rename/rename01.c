#include "incl.h"
#define MNT_POINT "mntpoint"

const char *old_file_name = "oldfile";

const char *old_dir_name = "olddir";

const char *new_file_name = "newfile";

const char *new_dir_name = "newdir";

struct stat old_file_st, old_dir_st, new_file_st, new_dir_st;

inline void swap(const char **a, const char **b)
{
	const char *tmp__ = *a;
}

void setup(void)
{
	chdir(MNT_POINT);
	touch(old_file_name, 0700, NULL);
	mkdir(old_dir_name, 00770);
	stat(old_file_name, &old_file_st);
	stat(old_dir_name, &old_dir_st);
}

void run(void)
{
	TST_EXP_PASS(rename(old_file_name, new_file_name),
						"rename(%s, %s)",
						old_file_name, new_file_name);
	TST_EXP_PASS(rename(old_dir_name, new_dir_name),
						"rename(%s, %s)",
						old_dir_name, new_dir_name);
	stat(new_file_name, &new_file_st);
	stat(new_dir_name, &new_dir_st);
	TST_EXP_EQ_LU(old_file_st.st_dev, new_file_st.st_dev);
	TST_EXP_EQ_LU(old_file_st.st_ino, new_file_st.st_ino);
	TST_EXP_EQ_LU(old_dir_st.st_dev, new_dir_st.st_dev);
	TST_EXP_EQ_LU(old_dir_st.st_ino, new_dir_st.st_ino);
	TST_EXP_FAIL(stat(old_file_name, &old_file_st),
				ENOENT,
				"stat(%s, &old_file_st)",
				old_file_name);
	TST_EXP_FAIL(stat(old_dir_name, &old_dir_st),
				ENOENT,
				"stat(%s, &old_dir_st)",
				old_dir_name);
	swap(&old_file_name, &new_file_name);
	swap(&old_dir_name, &new_dir_name);
}

void main(){
	setup();
	cleanup();
}
