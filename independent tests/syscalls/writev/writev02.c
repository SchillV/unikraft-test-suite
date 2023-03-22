#include "incl.h"
#define	K_1	8192
#define	NBUFS		2
#define	MAX_IOVEC	2
#define	DATA_FILE	"writev_data_file"
char buf1[K_1];
char buf2[K_1];
char *bad_addr = 0;
struct iovec wr_iovec[MAX_IOVEC] = {
	{(caddr_t) - 1, CHUNK},
	{NULL, 0},
};
char name[K_1], f_name[K_1];
int fd[2], in_sighandler;
char *buf_list[NBUFS];
char *TCID = "writev02";
int TST_TOTAL = 1;
void sighandler(int);
void l_seek(int, off_t, int);
void setup(void);
void cleanup(void);
int main(int argc, char **argv)
{
	int lc;
	int nbytes;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		buf_list[0] = buf1;
		buf_list[1] = buf2;
		memset(buf_list[0], 0, K_1);
		memset(buf_list[1], 0, K_1);
		if (signal(SIGTERM, sighandler) == SIG_ERR)
			tst_brkm(TFAIL | TERRNO, cleanup,
				 "signal(SIGTERM, ..)");
		if (signal(SIGPIPE, sighandler) == SIG_ERR)
			tst_brkm(TFAIL | TERRNO, cleanup,
				 "signal(SIGPIPE, ..)");
		if ((fd[0] = open(f_name, O_WRONLY | O_CREAT, 0666)) < 0)
			tst_brkm(TFAIL | TERRNO, cleanup,
				 "open(.., O_WRONLY|O_CREAT, ..) failed");
		else {
			l_seek(fd[0], K_1, 0);
			if ((nbytes = write(fd[0], buf_list[1], K_1)) != K_1)
				tst_brkm(TFAIL | TERRNO, cleanup,
					 "write failed");
		}
		if (close(fd[0]) == -1)
			tst_brkm(TFAIL | TERRNO, cleanup, "close failed");
		if ((fd[0] = open(f_name, O_RDWR, 0666)) < 0)
			tst_brkm(TFAIL | TERRNO, cleanup,
				 "open(.., O_RDWR, ..) failed");
		 * In this block we are trying to call writev() with invalid
		 * vector to be written in a sparse file. This will return
		 * EFAULT. At the same time, check should be made whether
		 * the scheduled write() with valid data at 8k th offset is
		 * done correctly or not.
		 */
		l_seek(fd[0], 0, 0);
writev(fd[0], wr_iovec, 2);
		if (TEST_RETURN < 0) {
			if (TEST_ERRNO == EFAULT) {
				tst_resm(TPASS, "Received EFAULT as expected");
			} else if (TEST_ERRNO != EFAULT) {
				tst_resm(TFAIL, "Expected EFAULT, got %d",
					 TEST_ERRNO);
			}
			l_seek(fd[0], K_1, 0);
			if ((nbytes = read(fd[0], buf_list[0], CHUNK)) != CHUNK) {
				tst_resm(TFAIL, "Expected nbytes = 64, got "
					 "%d", nbytes);
			} else {
				if (memcmp(buf_list[0], buf_list[1], CHUNK)
				    != 0)
					tst_resm(TFAIL, "Error: writev() "
						 "over wrote %s", f_name);
			}
		} else
			tst_resm(TFAIL, "Error writev returned a positive "
				 "value");
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, sighandler, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	strcpy(name, DATA_FILE);
	sprintf(f_name, "%s.%d", name, getpid());
	bad_addr = mmap(0, 1, PROT_NONE,
			MAP_PRIVATE_EXCEPT_UCLINUX | MAP_ANONYMOUS, 0, 0);
	if (bad_addr == MAP_FAILED)
		tst_brkm(TBROK | TERRNO, cleanup, "mmap failed");
	wr_iovec[0].iov_base = bad_addr;
}
void cleanup(void)
{
	close(fd[0]);
	close(fd[1]);
	if (munmap(bad_addr, 1) == -1)
		tst_resm(TWARN | TERRNO, "unmap failed");
	if (unlink(f_name) == -1)
		tst_resm(TWARN | TERRNO, "unlink failed");
	tst_rmdir();
}
void sighandler(int sig)
{
	switch (sig) {
	case SIGTERM:
		break;
	case SIGPIPE:
		++in_sighandler;
		return;
	default:
		tst_resm(TBROK, "sighandler received invalid signal : %d", sig);
		break;
	}
	if (unlink(f_name) == -1 && errno != ENOENT)
		tst_resm(TFAIL | TERRNO, "unlink failed");
}
void l_seek(int fdesc, off_t offset, int whence)
{
	if (lseek(fdesc, offset, whence) == -1)
		tst_resm(TBROK | TERRNO, "lseek failed");
}

