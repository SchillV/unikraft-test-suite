#include "incl.h"
extern int dummy_func_test(void);

int __init dummy_init(void)
{
	struct proc_dir_entry *proc_dummy;
	proc_dummy = proc_mkdir("dummy_dep", 0);
	dummy_func_test();
	return 0;
}

void __exit dummy_exit(void)
{
	remove_proc_entry("dummy_dep", 0);
}
module_init(dummy_init);
module_exit(dummy_exit);
MODULE_LICENSE("GPL");

void main(){
	setup();
	cleanup();
}
