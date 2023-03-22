#include "incl.h"
  *   Copyright (C) Bull S.A. 2001
  *   Copyright (c) International Business Machines  Corp., 2001
  *
  *   04/2002 Ported by Jacky Malcles
  */
#define TEST_TMPDIR	"chroot04_tmpdir"

int  verify_chroot(
{
	TST_EXP_FAIL(chroot(TEST_TMPDIR), EACCES, "no search permission chroot()");
}

void setup(void)
{
	struct passwd *ltpuser;
	mkdir(TEST_TMPDIR, 0222);
	ltpuser = getpwnam("nobody");
	seteuid(ltpuser->pw_uid);
}

void main(){
	setup();
	cleanup();
}
