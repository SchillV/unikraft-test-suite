#ifndef PRCTL06_H
#define PRCTL06_H
#include "incl.h"

#define PROC_STATUS        "/proc/self/status"
#define MNTPOINT           "mntpoint"
#define TESTBIN            "prctl06_execve"
#define TEST_REL_BIN_DIR   MNTPOINT"/"
#define BIN_PATH           MNTPOINT"/"TESTBIN
#define SUID_MODE          (S_ISUID|S_ISGID|S_IXUSR|S_IXGRP|S_IXOTH)

static void check_no_new_privs(int val, char *name, int flag)
{
	int ret = prctl(PR_GET_NO_NEW_PRIVS, 0, 0, 0, 0);
	if (ret == val)
		printf("%s prctl(PR_GET_NO_NEW_PRIVS) expected %d got %d\n",
			name, val, val);
	else
		printf("%s prctl(PR_GET_NO_NEW_PRIVS) expected %d got %d\n", name, val, ret);
}

#endif
