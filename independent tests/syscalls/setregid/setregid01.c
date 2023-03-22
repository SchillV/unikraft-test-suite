#include "incl.h"

gid_t gid, egid;

gid_t neg_one = -1;

struct tcase {
	gid_t *arg1;
	gid_t *arg2;
	const char *msg;
} tcases[] = {
	{&neg_one, &neg_one, "Leave real and effective both gids unchanged" },
	{&neg_one, &egid,    "Change effective to effective gid" },
	{&gid,     &neg_one, "Change real to real gid" },
	{&neg_one, &gid,     "Change effective to real gid" },
	{&gid,     &gid,     "Change real and effective both gids to current real gid" }
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	TST_EXP_PASS(SETREGID(*tc->arg1, *tc->arg2), "%s:", tc->msg);
}

void setup(void)
{
	gid = getgid();
	GID16_CHECK(gid, setregid);
	egid = getegid();
	GID16_CHECK(egid, setregid);
}

void main(){
	setup();
	cleanup();
}
