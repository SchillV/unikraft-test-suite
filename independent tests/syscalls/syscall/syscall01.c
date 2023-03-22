#include "incl.h"
#define _GNU_SOURCE

int  verify_getpid(
{
	pid_t p1, p2;
	p1 = getpid();
	p2 = syscall(SYS_getpid);
	if (p1 == p2) {
		tst_res(TPASS, "getpid() == syscall(SYS_getpid)");
	} else {
		tst_res(TFAIL, "getpid() = %i, syscall(SYS_getpid) = %i",
			p1, p2);
	}
}

int  verify_getuid(
{
	uid_t u1, u2;
	u1 = getuid();
	u2 = syscall(SYS_getuid);
	if (u1 == u2) {
		tst_res(TPASS, "getuid() == syscall(SYS_getuid)");
	} else {
		tst_res(TFAIL, "getuid() = %i, syscall(SYS_getuid) = %i",
			u1, u2);
	}
}

int  verify_getgid(
{
	gid_t g1, g2;
	g1 = getgid();
	g2 = syscall(SYS_getgid);
	if (g1 == g2) {
		tst_res(TPASS, "getgid() == syscall(SYS_getgid)");
	} else {
		tst_res(TFAIL, "getgid() = %i, syscall(SYS_getgid) = %i",
			g1, g2);
	}
}

void (*tcases[])(void) = {
int 	verify_getpid,
int 	verify_getuid,
int 	verify_getgid,
};

int  verify_syscall(unsigned int n)
{
	tcases[n]();
}

void main(){
	setup();
	cleanup();
}
