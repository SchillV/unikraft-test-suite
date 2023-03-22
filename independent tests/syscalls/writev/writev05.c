#include "incl.h"
#define	K_1	8192
#define	NBUFS		2
#define	MAX_IOVEC	2
#define	DATA_FILE	"writev_data_file"
char buf1[K_1];
char buf2[K_1];
char buf3[K_1];
char *bad_addr = 0;
struct iovec wr_iovec[MAX_IOVEC] = {
	{(caddr_t) - 1, CHUNK},
	{NULL, 0}
};
char name[K_1], f_name[K_1];
int fd[2], in_sighandler;
char *buf_list[NBUFS];
char *TCID = "writev05";
int TST_TOTAL = 1;
void sighandler(int);
long l_seek(int, long, int);
void setup(void);
void cleanup(void);
#if !defined(UCLINUX)
int main(int argc, char **argv)
{
	int lc;
	int nbytes;
	tst_parse_opts(argc, argv, NULL, NULL);
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		buf_list[0] = buf1;
		buf_list[1] = buf2;
		if (signal(SIGTERM, sighandler) == SIG_ERR) {
			perror("signal");
			tst_resm(TFAIL, "signal() SIGTERM FAILED");
			cleanup();
		}
		if (signal(SIGPIPE, sighandler) == SIG_ERR) {
			perror("signal");
			tst_resm(TFAIL, "signal() SIGPIPE FAILED");
			cleanup();
		}
		memset(buf_list[0], 0, K_1);
		memset(buf_list[1], 0, K_1);
		if ((fd[0] = open(f_name, O_WRONLY | O_CREAT, 0666)) < 0) {
			tst_resm(TFAIL, "open(2) failed: fname = %s, "
				 "errno = %d", f_name, errno);
			cleanup();
		} else {
			if ((nbytes = write(fd[0], buf_list[1], K_1)) != K_1) {
				tst_resm(TFAIL, "write(2) failed: nbytes "
					 "= %d, errno = %d", nbytes, errno);
				cleanup();
			}
		}
		if (close(fd[0]) < 0) {
			tst_resm(TFAIL, "close failed: errno = %d", errno);
			cleanup();
		}
		if ((fd[0] = open(f_name, O_RDWR, 0666)) < 0) {
			tst_resm(TFAIL, "open failed: fname = %s, errno = %d",
				 f_name, errno);
			cleanup();
		}
		 * In this block we are trying to call writev() with invalid
		 * vector to be written in a sparse file. This will return
		 * EFAULT. At the same time, check should be made whether
		 * the scheduled write() with valid data is done correctly
		 * or not.
		 */
		l_seek(fd[0], 0, 0);
writev(fd[0], wr_iovec, 2);
		if (TEST_RETURN < 0) {
			if (TEST_ERRNO == EFAULT) {
				tst_resm(TPASS, "Received EFAULT as expected");
			} else {
				tst_resm(TFAIL, "Expected EFAULT, got %d",
					 TEST_ERRNO);
			}
			l_seek(fd[0], K_1, 0);
			if ((nbytes = read(fd[0], buf_list[0], CHUNK)) != 0) {
				tst_resm(TFAIL, "Expected nbytes = 0, got "
					 "%d", nbytes);
			}
		} else {
			tst_resm(TFAIL, "Error writev returned a positive "
				 "value");
		}
	}
	close(fd[0]);
	close(fd[1]);
	cleanup();
	tst_exit();
}
#else
int main(void)
{
	tst_resm(TINFO, "test is not available on uClinux");
	tst_exit();
}
void setup(void)
{
	tst_sig(FORK, DEF_HANDLER, cleanup);
	 * TEST_PAUSE contains the code to fork the test with the -i option.
	 * You want to make sure you do this before you create your temporary
	 * directory.
	 */
	TEST_PAUSE;
	tst_tmpdir();
	strcpy(name, DATA_FILE);
	sprintf(f_name, "%s.%d", name, getpid());
	bad_addr = mmap(0, 1, PROT_NONE,
			MAP_PRIVATE_EXCEPT_UCLINUX | MAP_ANONYMOUS, 0, 0);
	if (bad_addr == MAP_FAILED) {
		printf("mmap failed\n");
	}
	wr_iovec[0].iov_base = bad_addr;
}
void cleanup(void)
{
	if (unlink(f_name) < 0) {
		tst_resm(TFAIL, "unlink Failed--file = %s, errno = %d",
			 f_name, errno);
	}
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
		tst_resm(TFAIL, "sighandler() received invalid signal "
			 ": %d", sig);
		break;
	}
	if ((unlink(f_name) < 0) && (errno != ENOENT)) {
		tst_resm(TFAIL, "unlink Failed--file = %s, errno = %d",
			 f_name, errno);
		cleanup();
	}
	exit(sig);
}
long l_seek(int fdesc, long offset, int whence)
{
	if (lseek(fdesc, offset, whence) < 0) {
		tst_resm(TFAIL, "lseek Failed : errno = %d", errno);
	}
	return 0;
}

