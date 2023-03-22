#include "incl.h"
#define MODULE_NAME	"init_module.ko"

struct stat sb;

void *buf;

void setup(void)
{
	int fd;
	tst_module_exists(MODULE_NAME, NULL);
	fd = open(MODULE_NAME, O_RDONLY|O_CLOEXEC);
	fstat(fd, &sb);
	buf = mmap(0, sb.st_size, PROT_READ|PROT_EXEC, MAP_PRIVATE, fd, 0);
	close(fd);
}

void run(void)
{
	TST_EXP_PASS(init_module(buf, sb.st_size, "status=valid"));
	if (!TST_PASS)
		return;
	tst_module_unload(MODULE_NAME);
}

void cleanup(void)
{
	munmap(buf, sb.st_size);
}

void main(){
	setup();
	cleanup();
}
