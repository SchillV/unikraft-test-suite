#include "incl.h"
int dummy_func_test(void)
{
	return 0;
}
EXPORT_SYMBOL(dummy_func_test);

int __init dummy_init(void)
{
	struct proc_dir_entry *proc_dummy;
	proc_dummy = proc_mkdir("dummy", 0);
	return 0;
}

void __exit dummy_exit(void)
{
	remove_proc_entry("dummy", 0);
}
module_init(dummy_init);
module_exit(dummy_exit);
MODULE_LICENSE("GPL");

void main(){
	setup();
	cleanup();
}
