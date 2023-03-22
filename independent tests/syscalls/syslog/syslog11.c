#include "incl.h"
struct tcase {
	int type;
	char *buf;
	int len;
	char *desc;
};

char buf;
#define syslog(arg1, arg2, arg3) tst_syscall(__NR_syslog, arg1, arg2, arg3)

struct tcase tcases[] = {
	{0, &buf, 0, "type 0/Close the log"},
	{1, &buf, 0, "type 1/Open the log"},
	{2, &buf, 0, "type 2/Read from the log"},
	{3, &buf, 0, "type 3/Read ring buffer"},
	 * Next two lines will clear dmesg.
	 * Uncomment if that is okay. -Robbie Williamson
	 */
	 * { 4, &buf, 0, "type 4/Read and clear ring buffer" },
	 * { 5, &buf, 0, "type 5/Clear ring buffer" },
	 */
	{8, NULL, 1, "type 8/Set log level to 1"},
	{8, NULL, 7, "type 8/Set log level to 7(default)"},
	{6, NULL, 0, "type 6/Disable printk's to console"},
	{7, NULL, 0, "type 7/Enable printk's to console"},
};

void run(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	TST_EXP_PASS(syslog(tc->type, tc->buf, tc->len),
			"syslog() with %s", tc->desc);
}

void main(){
	setup();
	cleanup();
}
