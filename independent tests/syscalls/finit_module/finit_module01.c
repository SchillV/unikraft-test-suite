#include "incl.h"
#define MODULE_NAME	"finit_module.ko"

int fd;

char *mod_path;

void setup(void)
{
	tst_module_exists(MODULE_NAME, &mod_path);
	fd = open(mod_path, O_RDONLY|O_CLOEXEC);
}

void run(void)
{
	TST_EXP_PASS(finit_module(fd, "status=valid", 0));
	if (!TST_PASS)
		return;
	tst_module_unload(MODULE_NAME);
}

void cleanup(void)
{
	close(fd);
}

void main(){
	setup();
	cleanup();
}
