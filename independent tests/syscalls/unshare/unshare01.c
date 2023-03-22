#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_UNSHARE

struct test_case_t {
	int mode;
	const char *desc;
} tc[] = {
	{CLONE_FILES,	"CLONE_FILES"},
	{CLONE_FS,	"CLONE_FS"},
	{CLONE_NEWNS,	"CLONE_NEWNS"},
};

void run(unsigned int i)
{
	pid_t pid = fork();
	if (pid == 0)
		TST_EXP_PASS(unshare(tc[i].mode), "unshare(%s)", tc[i].desc);
}

void main(){
	setup();
	cleanup();
}
