#include "incl.h"
* Copyright (c) 2016 Fujitsu Ltd.
* Author: Xiao Yang <yangx.jy@cn.fujitsu.com>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of version 2 of the GNU General Public License as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it would be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* You should have received a copy of the GNU General Public License
* alone with this program.
*/
* Test Name: removexattr01
*
* Description:
* The testcase checks the basic functionality of the removexattr(2).
* removexattr(2) removes the extended attribute identified by
* name and associated with the given path in the filesystem.
*/
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
char *TCID = "removexattr01";
#ifdef HAVE_SYS_XATTR_H
#define USER_KEY	"user.test"
#define VALUE	"test"
#define VALUE_SIZE	(sizeof(VALUE) - 1)

int  verify_removexattr(

void setup(void);

void cleanup(void);
int TST_TOTAL = 1;
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
int 		verify_removexattr();
	}
	cleanup();
	tst_exit();
}

int  verify_removexattr(
{
	int n;
	int size = 64;
	char buf[size];
	n = setxattr("testfile", USER_KEY, VALUE, VALUE_SIZE, XATTR_CREATE);
	if (n == -1) {
		if (errno == ENOTSUP) {
			tst_brkm(TCONF, cleanup, "no xattr support in fs or "
				 "mounted without user_xattr option");
		} else {
			tst_brkm(TFAIL | TERRNO, cleanup, "setxattr() failed");
		}
	}
removexattr("testfile", USER_KEY);
	if (TEST_RETURN != 0) {
		tst_resm(TFAIL | TTERRNO, "removexattr() failed");
		return;
	}
	n = getxattr("testfile", USER_KEY, buf, size);
	if (n != -1) {
		tst_resm(TFAIL, "getxattr() succeeded for deleted key");
		return;
	}
	if (errno != ENODATA) {
		tst_resm(TFAIL | TTERRNO, "getxattr() failed unexpectedly");
	} else {
		tst_resm(TPASS, "removexattr() succeeded");
	}
}

void setup(void)
{
	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	touch(cleanup, "testfile", 0644, NULL);
}

void cleanup(void)
{
	tst_rmdir();
}
int main(int ac, char **av)
{
	tst_brkm(TCONF, NULL, "<sys/xattr.h> does not exist.");
}
#endif

