#include "incl.h"
#define DUMMY_MOD		"dummy_del_mod"
#define DUMMY_MOD_KO		"dummy_del_mod.ko"
#define DUMMY_MOD_DEP_KO	"dummy_del_mod_dep.ko"

int dummy_mod_loaded;

int dummy_mod_dep_loaded;

void do_delete_module(void)
{
tst_syscall(__NR_delete_module, DUMMY_MOD, 0);
	if (TST_RET < 0) {
		if (TST_ERR == EWOULDBLOCK) {
			tst_res(TPASS | TTERRNO,
				"delete_module() failed as expected");
		} else {
			tst_res(TFAIL | TTERRNO, "delete_module() failed "
			"unexpectedly; expected: %s",
			tst_strerrno(EWOULDBLOCK));
		}
	} else {
		tst_res(TFAIL, "delete_module() succeeded unexpectedly");
		dummy_mod_loaded = 0;
		 * insmod DUMMY_MOD_KO again in case running
		 * with -i option
		 */
		tst_module_load(DUMMY_MOD_KO, NULL);
		dummy_mod_loaded = 1;
	}
}

void setup(void)
{
	tst_module_load(DUMMY_MOD_KO, NULL);
	dummy_mod_loaded = 1;
	tst_module_load(DUMMY_MOD_DEP_KO, NULL);
	dummy_mod_dep_loaded = 1;
}

void cleanup(void)
{
	if (dummy_mod_dep_loaded == 1)
		tst_module_unload(DUMMY_MOD_DEP_KO);
	if (dummy_mod_loaded == 1)
		tst_module_unload(DUMMY_MOD_KO);
}

void main(){
	setup();
	cleanup();
}
