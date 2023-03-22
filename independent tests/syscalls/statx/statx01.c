#include "incl.h"
#define _GNU_SOURCE
#define TESTFILE "test_file"
#define MNTPOINT "mntpoint/"
#define DEVICEFILE MNTPOINT"blk_dev"
#define MODE 0644
#define SIZE 256
#define MAJOR 8
#define MINOR 1

int file_fd = -1;
#ifdef HAVE_STRUCT_STATX_STX_MNT_ID

void test_mnt_id(struct statx *buf)
{
	FILE *file;
	char line[PATH_MAX];
	int pid, flag = 0;
	unsigned int line_mjr, line_mnr;
	uint64_t mnt_id;
	if (!(buf->stx_mask & STATX_MNT_ID)) {
		tst_res(TCONF, "stx_mnt_id is not supported until linux 5.8");
		return;
	}
	file = fopen("/proc/self/mountinfo", "r");
	while (fgets(line, sizeof(line), file)) {
		if (sscanf(line, "%"SCNu64" %*d %d:%d", &mnt_id, &line_mjr, &line_mnr) != 3)
			continue;
		if (line_mjr == buf->stx_dev_major && line_mnr == buf->stx_dev_minor) {
			if (buf->stx_mnt_id == mnt_id) {
				flag = 1;
				break;
			}
			tst_res(TINFO, "%s doesn't contain %"PRIu64" %d:%d",
				line, (uint64_t)buf->stx_mnt_id, buf->stx_dev_major, buf->stx_dev_minor);
		}
	}
	fclose(file);
	if (flag)
		tst_res(TPASS,
			"statx.stx_mnt_id equals to mount_id(%"PRIu64") in /proc/self/mountinfo",
			mnt_id);
	else
		tst_res(TFAIL,
			"statx.stx_mnt_id(%"PRIu64") doesn't exist in /proc/self/mountinfo",
			(uint64_t)buf->stx_mnt_id);
	pid = getpid();
	snprintf(line, PATH_MAX, "/proc/%d/fdinfo/%d", pid, file_fd);
	TST_ASSERT_FILE_INT(line, "mnt_id:", buf->stx_mnt_id);
}
#endif

void test_normal_file(void)
{
	struct statx buff;
statx(AT_FDCWD, TESTFILE, 0, 0, &buff);
	if (TST_RET == 0)
		tst_res(TPASS,
			"statx(AT_FDCWD, %s, 0, 0, &buff)", TESTFILE);
	else
		tst_brk(TFAIL | TTERRNO,
			"statx(AT_FDCWD, %s, 0, 0, &buff)", TESTFILE);
	if (geteuid() == buff.stx_uid)
		tst_res(TPASS, "stx_uid(%u) is correct", buff.stx_uid);
	else
		tst_res(TFAIL, "stx_uid(%u) is different from euid(%u)",
			buff.stx_uid, geteuid());
	if (getegid() == buff.stx_gid)
		tst_res(TPASS, "stx_gid(%u) is correct", buff.stx_gid);
	else
		tst_res(TFAIL, "stx_gid(%u) is different from egid(%u)",
			buff.stx_gid, getegid());
	if (buff.stx_size == SIZE)
		tst_res(TPASS,
			"stx_size(%"PRIu64") is correct", (uint64_t)buff.stx_size);
	else
		tst_res(TFAIL,
			"stx_size(%"PRIu64") is different from expected(%u)",
			(uint64_t)buff.stx_size, SIZE);
	if ((buff.stx_mode & ~(S_IFMT)) == MODE)
		tst_res(TPASS, "stx_mode(%u) is correct", buff.stx_mode);
	else
		tst_res(TFAIL, "stx_mode(%u) is different from expected(%u)",
			buff.stx_mode, MODE);
	if (buff.stx_blocks <= buff.stx_blksize/512 * 2)
		tst_res(TPASS, "stx_blocks(%"PRIu64") is valid",
			(uint64_t)buff.stx_blocks);
	else
		tst_res(TFAIL, "stx_blocks(%"PRIu64") is invalid",
			(uint64_t)buff.stx_blocks);
	if (buff.stx_nlink == 1)
		tst_res(TPASS, "stx_nlink(1) is correct");
	else
		tst_res(TFAIL, "stx_nlink(%u) is different from expected(1)",
			buff.stx_nlink);
#ifdef HAVE_STRUCT_STATX_STX_MNT_ID
	test_mnt_id(&buff);
#else
	tst_res(TCONF, "stx_mnt_id is not defined in struct statx");
#endif
}

void test_device_file(void)
{
	struct statx buff;
statx(AT_FDCWD, DEVICEFILE, 0, 0, &buff);
	if (TST_RET == 0)
		tst_res(TPASS,
			"statx(AT_FDCWD, %s, 0, 0, &buff)", DEVICEFILE);
	else
		tst_brk(TFAIL | TTERRNO,
			"statx(AT_FDCWD, %s, 0, 0, &buff)", DEVICEFILE);
	if (buff.stx_rdev_major == MAJOR)
		tst_res(TPASS, "stx_rdev_major(%u) is correct",
			buff.stx_rdev_major);
	else
		tst_res(TFAIL,
			"stx_rdev_major(%u) is different from expected(%u)",
			buff.stx_rdev_major, MAJOR);
	if (buff.stx_rdev_minor == MINOR)
		tst_res(TPASS, "stx_rdev_minor(%u) is correct",
			buff.stx_rdev_minor);
	else
		tst_res(TFAIL,
			"stx_rdev_minor(%u) is different from expected(%u)",
			buff.stx_rdev_minor, MINOR);
}

struct tcase {
	void (*tfunc)(void);
} tcases[] = {
	{&test_normal_file},
	{&test_device_file}
};

void run(unsigned int i)
{
	tcases[i].tfunc();
}

void setup(void)
{
	char data_buff[SIZE];
	umask(0);
	memset(data_buff, '@', sizeof(data_buff));
	file_fd =  open(TESTFILE, O_RDWR|O_CREAT, MODE);
	write(1, file_fd, data_buff, sizeof1, file_fd, data_buff, sizeofdata_buff));
	mknod(DEVICEFILE, S_IFBLK | 0777, makedevDEVICEFILE, S_IFBLK | 0777, makedevMAJOR, MINOR));
}

void cleanup(void)
{
	if (file_fd > -1)
		close(file_fd);
}

void main(){
	setup();
	cleanup();
}
