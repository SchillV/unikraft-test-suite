#include "incl.h"

char status[20];
module_param_string(status, status, 20, 0444);

int dummy_init(void)
{
	struct proc_dir_entry *proc_dummy;
	if (!strcmp(status, "invalid"))
		return -EINVAL;
	proc_dummy = proc_mkdir("dummy", 0);
	return 0;
}
module_init(dummy_init);

void dummy_exit(void)
{
	remove_proc_entry("dummy", 0);
}
module_exit(dummy_exit);
MODULE_LICENSE("GPL");

void main(){
	setup();
	cleanup();
}
