#include "incl.h"
#define _GNU_SOURCE
#define MNTPOINT	"mntpoint"
#define TEST_FILE	MNTPOINT"/test_file"
#define LARGE_FILE	"large_file"
#define DIR_MODE 0755
char *TCID = "open12";

const char *device;

unsigned int mount_flag, skip_noatime;

void setup(void);

void cleanup(void);

void test_append(void);

void test_noatime(void);

void test_cloexec(void);

void test_largefile(void);

void (*test_func[])(void) = { test_append, test_noatime, test_cloexec,
				     test_largefile };
int TST_TOTAL = ARRAY_SIZE(test_func);
int main(int argc, char **argv)
{
	int lc;
	int i;
	tst_parse_opts(argc, argv, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		for (i = 0; i < TST_TOTAL; i++)
			(*test_func[i])();
	}
	cleanup();
	tst_exit();
}

void setup(void)
{
	const char *mount_flags[] = {"noatime", "relatime", NULL};
	TEST_PAUSE;
	tst_sig(FORK, DEF_HANDLER, cleanup);
	tst_tmpdir();
	mkdir(cleanup, MNTPOINT, DIR_MODE);
	if (tst_path_has_mnt_flags(cleanup, NULL, mount_flags)) {
		const char *fs_type;
		fs_type = tst_dev_fs_type();
		device = tst_acquire_device(cleanup);
		if (!device) {
			tst_resm(TINFO, "Failed to obtain block device");
			skip_noatime = 1;
			goto end;
		}
		tst_mkfs(cleanup, device, fs_type, NULL, NULL);
		mount(cleanup, device, MNTPOINT, fs_type, MS_STRICTATIME, NULL);
		mount_flag = 1;
	}
end:
	fprintf(cleanup, TEST_FILE, TEST_FILE);
}

void test_append(void)
{
	off_t len1, len2;
open(TEST_FILE, O_RDWR | O_APPEND, 0777);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL | TTERRNO, "open failed");
		return;
	}
	len1 = lseek(cleanup, TEST_RETURN, 0, SEEK_CUR);
	write(cleanup, 1, TEST_RETURN, TEST_FILE,
		sizeof(TEST_FILE));
	len2 = lseek(cleanup, TEST_RETURN, 0, SEEK_CUR);
	close(cleanup, TEST_RETURN);
	if (len2 > len1)
		tst_resm(TPASS, "test O_APPEND for open success");
	else
		tst_resm(TFAIL, "test O_APPEND for open failed");
}

void test_noatime(void)
{
	char read_buf;
	struct stat old_stat, new_stat;
	if (skip_noatime) {
		tst_resm(TCONF,
		         "test O_NOATIME flag for open needs filesystems which "
		         "is mounted without noatime and relatime");
		return;
	}
	stat(cleanup, TEST_FILE, &old_stat);
	sleep(1);
open(TEST_FILE, O_RDONLY | O_NOATIME, 0777);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL | TTERRNO, "open failed");
		return;
	}
	read(cleanup, 1, TEST_RETURN, &read_buf, 1);
	close(cleanup, TEST_RETURN);
	stat(cleanup, TEST_FILE, &new_stat);
	if (old_stat.st_atime == new_stat.st_atime)
		tst_resm(TPASS, "test O_NOATIME for open success");
	else
		tst_resm(TFAIL, "test O_NOATIME for open failed");
}

void test_cloexec(void)
{
	pid_t pid;
	int status;
	char buf[20];
open(TEST_FILE, O_RDWR | O_APPEND | O_CLOEXEC, 0777);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL | TTERRNO, "open failed");
		return;
	}
	sprintf(buf, "%ld", TEST_RETURN);
	pid = tst_fork();
	if (pid < 0)
		tst_brkm(TBROK | TERRNO, cleanup, "fork() failed");
	if (pid == 0) {
		if (execlp("open12_child", "open12_child", buf, NULL))
			exit(2);
	}
	close(cleanup, TEST_RETURN);
	if (wait(&status) != pid)
		tst_brkm(TBROK | TERRNO, cleanup, "wait() failed");
	if (WIFEXITED(status)) {
		switch ((int8_t)WEXITSTATUS(status)) {
		case 0:
			tst_resm(TPASS, "test O_CLOEXEC for open success");
		break;
		case 1:
			tst_resm(TFAIL, "test O_CLOEXEC for open failed");
		break;
		default:
			tst_brkm(TBROK, cleanup, "execlp() failed");
		}
	} else {
		tst_brkm(TBROK, cleanup,
				 "open12_child exits with unexpected error");
	}
}

void test_largefile(void)
{
	int fd;
	off_t offset;
	fd = open(cleanup, LARGE_FILE,
				O_LARGEFILE | O_RDWR | O_CREAT, 0777);
	offset = lseek(fd, 4.1*1024*1024*1024, SEEK_SET);
	if (offset == -1)
		tst_brkm(TBROK | TERRNO, cleanup, "lseek failed");
	write(cleanup, 1, fd, LARGE_FILE,
		sizeof(LARGE_FILE));
	close(cleanup, fd);
open(LARGE_FILE, O_LARGEFILE | O_RDONLY, 0777);
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL, "test O_LARGEFILE for open failed");
	} else {
		tst_resm(TPASS, "test O_LARGEFILE for open success");
		close(cleanup, TEST_RETURN);
	}
}

void cleanup(void)
{
	if (mount_flag && tst_umount(MNTPOINT) == -1)
		tst_brkm(TWARN | TERRNO, NULL, "umount(2) failed");
	if (device)
		tst_release_device(device);
	tst_rmdir();
}

