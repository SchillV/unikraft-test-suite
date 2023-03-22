#include "incl.h"
#define MODULE_NAME	"dummy_del_mod"
#define MODULE_NAME_KO	"dummy_del_mod.ko"

int module_loaded;

void do_delete_module(void)
{
	if (module_loaded == 0) {
		tst_module_load(MODULE_NAME_KO, NULL);
		module_loaded = 1;
	}
tst_syscall(__NR_delete_module, MODULE_NAME, 0);
	if (TST_RET == -1) {
		tst_res(TFAIL | TTERRNO, "delete_module() failed to "
			"remove module entry for %s ", MODULE_NAME);
		return;
	}
	tst_res(TPASS, "delete_module() successful");
	module_loaded = 0;
}

void cleanup(void)
{
	if (module_loaded == 1)
		tst_module_unload(MODULE_NAME_KO);
}

void main(){
	setup();
	cleanup();
}
