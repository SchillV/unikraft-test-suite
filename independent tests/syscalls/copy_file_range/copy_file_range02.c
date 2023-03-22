#include "incl.h"
#define _GNU_SOURCE

int fd_src;

int fd_dest;

int fd_rdonly;

int fd_dir;

int fd_closed;

int fd_append;

int fd_immutable;

int fd_swapfile;

int fd_dup;

int fd_blkdev;

int fd_chrdev;

int fd_fifo;

int fd_pipe[2];

int fd_copy;

int need_unlink;

int chattr_i_nsup;

int swap_nsup;

int cross_sup;

int loop_devn;

struct tcase {
	int	*copy_to_fd;
	int	flags;
	int	exp_err;
	loff_t     len;
	const char *tname;
	int     new_error;
} tcases[] = {
	{&fd_rdonly,	0,	EBADF,		CONTSIZE,	"readonly file",	0},
	{&fd_dir,	0,	EISDIR,		CONTSIZE,	"directory",	0},
	{&fd_append,	0,	EBADF,		CONTSIZE,	"append to file",	0},
	{&fd_closed,	0,	EBADF,		CONTSIZE,	"closed file descriptor",	0},
	{&fd_dest,	-1,	EINVAL,		CONTSIZE,	"invalid flags",	0},
	{&fd_immutable,	0,	EPERM,		CONTSIZE,	"immutable file",	1},
	{&fd_swapfile,	0,	ETXTBSY,	CONTSIZE,	"swap file",	1},
	{&fd_dup,	0,	EINVAL,		CONTSIZE/2,	"overlaping range",	1},
	{&fd_blkdev,	0,	EINVAL,		CONTSIZE,	"block device", 	0},
	{&fd_chrdev,	0,	EINVAL,		CONTSIZE,	"char device",	0},
	{&fd_fifo,	0,	EINVAL,		CONTSIZE,	"fifo", 	0},
	{&fd_pipe[0],	0,	EINVAL,		CONTSIZE,	"pipe", 	0},
#ifdef TST_ABI64
	{&fd_copy,	0,	EOVERFLOW,	ULLONG_MAX,	"max length", 	1},
#else
	{&fd_copy,	0,	EFBIG,		ULLONG_MAX,	"max length", 	1},
#endif
	{&fd_copy,	0,	EFBIG,		MIN_OFF,	"max file size", 	1},
};

int run_command(char *command, char *option, char *file)
{
	const char *const cmd[] = {command, option, file, NULL};
	int ret;
	ret = tst_cmd(cmd, NULL, NULL, TST_CMD_PASS_RETVAL);
	switch (ret) {
	case 0:
	return 0;
	case 255:
		tst_res(TCONF, "%s binary not installed or failed", command);
	return 1;
	default:
		tst_res(TCONF, "%s exited with %i", command, ret);
	return 2;
	}
}

int  verify_copy_file_range(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	loff_t dst = 0;
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if (tc->new_error && !cross_sup) {
		tst_res(TCONF,
			"copy_file_range() doesn't support cross-device, skip it");
		return;
	}
	if (tc->copy_to_fd == &fd_immutable && chattr_i_nsup) {
		tst_res(TCONF, "filesystem doesn't support chattr +i, skip it");
		return;
	}
	if (tc->copy_to_fd == &fd_swapfile && swap_nsup) {
		tst_res(TCONF, "filesystem doesn't support swapfile, skip it");
		return;
	}
	if (tc->copy_to_fd == &fd_blkdev && loop_devn == -1) {
		tst_res(TCONF, "filesystem doesn't have free loopdev, skip it");
		return;
	}
	if (tc->copy_to_fd == &fd_copy)
		dst = tst_max_lfs_filesize();
sys_copy_file_range(fd_src, 0, *tc->copy_to_f;
				&dst, tc->len, tc->flags));
	if (TST_RET == -1) {
		if (tc->exp_err == TST_ERR) {
			tst_res(TPASS | TTERRNO,
					"copy_file_range failed as expected");
		} else {
			tst_res(TFAIL | TTERRNO,
				"copy_file_range failed unexpectedly; expected %s, but got",
				tst_strerrno(tc->exp_err));
			return;
		}
	} else {
		tst_res(TFAIL,
			"copy_file_range returned wrong value: %ld", TST_RET);
	}
}

void cleanup(void)
{
	if (fd_append > 0)
		close(fd_append);
	if (fd_dir > 0)
		close(fd_dir);
	if (fd_rdonly > 0)
		close(fd_rdonly);
	if (fd_dest > 0)
		close(fd_dest);
	if (fd_src > 0)
		close(fd_src);
	if (fd_immutable > 0) {
		run_command("chattr", "-i", FILE_IMMUTABLE_PATH);
		close(fd_immutable);
	}
	if (fd_swapfile > 0) {
		run_command("swapoff", FILE_SWAP_PATH, NULL);
		close(fd_swapfile);
	}
	if (fd_dup > 0)
		close(fd_dup);
	if (fd_copy > 0)
		close(fd_copy);
	if (need_unlink > 0)
		unlink(FILE_FIFO);
	if (fd_pipe[0] > 0) {
		close(fd_pipe[0]);
		close(fd_pipe[1]);
	}
}

void setup(void)
{
	syscall_info();
	char dev_path[1024];
int 	cross_sup = verify_cross_fs_copy_support(FILE_SRC_PATH, FILE_MNTED_PATH);
	if (access(FILE_DIR_PATH, F_OK) == -1)
		mkdir(FILE_DIR_PATH, 0777);
	 * call tst_find_free_loopdev(), avoid overwriting its
	 * content on used loopdev.
	 */
	loop_devn = tst_find_free_loopdev(dev_path, sizeof(dev_path));
	mknod(FILE_FIFO, S_IFIFO | 0777, 0);
	need_unlink = 1;
	fd_src    = open(FILE_SRC_PATH, O_RDWR | O_CREAT, 0664);
	fd_dest   = open(FILE_DEST_PATH, O_RDWR | O_CREAT, 0664);
	fd_rdonly = open(FILE_RDONL_PATH, O_RDONLY | O_CREAT, 0664);
	fd_dir    = open(FILE_DIR_PATH, O_DIRECTORY);
	fd_closed = -1;
	fd_append = open(FILE_DEST_PATH,
			O_RDWR | O_CREAT | O_APPEND, 0664);
	fd_immutable = open(FILE_IMMUTABLE_PATH, O_RDWR | O_CREAT, 0664);
	fd_swapfile = open(FILE_SWAP_PATH, O_RDWR | O_CREAT, 0600);
	if (loop_devn != -1)
		fd_blkdev = open(dev_path, O_RDWR, 0600);
	fd_chrdev = open(FILE_CHRDEV, O_RDWR, 0600);
	fd_fifo = open(FILE_FIFO, O_RDWR, 0600);
	pipe(fd_pipe);
	write(1, fd_src, CONTENT, CONTSIZE);
	close(fd_src);
	fd_src = open(FILE_SRC_PATH, O_RDONLY, 0664);
	fd_dup = open(FILE_SRC_PATH, O_WRONLY|O_CREAT, 0666);
	fd_copy = open(FILE_COPY_PATH, O_RDWR | O_CREAT | O_TRUNC, 0664);
	chattr_i_nsup = run_command("chattr", "+i", FILE_IMMUTABLE_PATH);
	if (!tst_fs_has_free(".", sysconf(_SC_PAGESIZE) * 10, TST_BYTES)) {
		tst_res(TCONF, "Insufficient disk space to create swap file");
		swap_nsup = 3;
		return;
	}
	if (tst_fill_file(FILE_SWAP_PATH, 0, sysconf(_SC_PAGESIZE), 10) != 0) {
		tst_res(TCONF, "Failed to create swapfile");
		swap_nsup = 4;
		return;
	}
	swap_nsup = run_command("mkswap", FILE_SWAP_PATH, NULL);
	swap_nsup = run_command("swapon", FILE_SWAP_PATH, NULL);
}

void main(){
	setup();
	cleanup();
}
