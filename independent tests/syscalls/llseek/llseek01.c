#include "incl.h"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define TEMP_FILE	"tmp_file"
#define FILE_MODE	0644

char write_buff[BUFSIZ];

int fildes;

int  verify_llseek(
{
lseek(fildes, (loff_t) (80 * BUFSIZ), SEEK_SET);
	if (TST_RET == (80 * BUFSIZ))
		tst_res(TPASS, "lseek() can set file pointer position larger than file size limit");
	else
		tst_res(TFAIL, "lseek() returned wrong value %ld when write past file size", TST_RET);
	if (write(fildes, write_buff, BUFSIZ) == -1)
		tst_res(TPASS,"write failed after file size limit");
	else
		tst_brk(TFAIL, "write successful after file size limit");
lseek(fildes, (loff_t) BUFSIZ, SEEK_SET);
	if (TST_RET == BUFSIZ)
		tst_res(TPASS,"lseek() can set file pointer position under filer size limit");
	else
		tst_brk(TFAIL,"lseek() returns wrong value %ld when write under file size", TST_RET);
	if (write(fildes, write_buff, BUFSIZ) != -1)
		tst_res(TPASS, "write succcessfully under file size limit");
	else
		tst_brk(TFAIL, "write failed under file size limit");
	if (write(fildes, write_buff, BUFSIZ) == -1)
		tst_res(TPASS, "write failed after file size limit");
	else
		tst_brk(TFAIL, "write successfully after file size limit");
}

void setup(void)
{
	struct sigaction act;
	struct rlimit rlp;
	act.sa_handler = SIG_IGN;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGXFSZ, &act, NULL);
	rlp.rlim_cur = rlp.rlim_max = 2 * BUFSIZ;
	setrlimit(RLIMIT_FSIZE, &rlp);
	fildes = open(TEMP_FILE, O_RDWR | O_CREAT, FILE_MODE);
	write(1, fildes, write_buff, BUFSIZ);
}

void main(){
	setup();
	cleanup();
}
