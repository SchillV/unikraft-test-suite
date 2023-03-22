#include "incl.h"
#define CMD_DESC(x) .cmd = x, .desc = #x

struct tcase {
	const char *desc;
	int cmd;
} tcases[] = {
	{CMD_DESC(LINUX_REBOOT_CMD_CAD_ON)},
	{CMD_DESC(LINUX_REBOOT_CMD_CAD_OFF)},
};

void run(unsigned int n)
{
	TST_EXP_PASS(reboot(tcases[n].cmd), "reboot(%s)", tcases[n].desc);
}

void main(){
	setup();
	cleanup();
}
