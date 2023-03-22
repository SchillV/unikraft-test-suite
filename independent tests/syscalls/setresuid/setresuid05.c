#include "incl.h"
#define TEMP_FILE	"testfile"

struct passwd *ltpuser;

void setup(void)
{
	ltpuser = getpwnam("nobody");
	UID16_CHECK(ltpuser->pw_uid, "setresuid");
}

void run(void)
{
	struct stat buf;
	TST_EXP_PASS(SETRESUID(-1, ltpuser->pw_uid, -1));
	touch(TEMP_FILE, 0644, NULL);
	stat(TEMP_FILE, &buf);
	TST_EXP_EQ_LI(ltpuser->pw_uid, buf.st_uid);
	unlink(TEMP_FILE);
	TST_EXP_PASS_SILENT(SETRESUID(-1, 0, -1));
}

void main(){
	setup();
	cleanup();
}
