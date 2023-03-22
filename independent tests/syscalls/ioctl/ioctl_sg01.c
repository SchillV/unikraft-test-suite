#include "incl.h"
#define BUF_SIZE (128 * 4096)
#define CMD_SIZE 6

int devfd = -1;

char buffer[BUF_SIZE];

unsigned char command[CMD_SIZE];

struct sg_io_hdr query;

const char *find_generic_scsi_device(int access_flags)
{
	DIR *devdir;
	struct dirent *ent;
	int tmpfd;
	

char devpath[PATH_MAX];
	errno = 0;
	devdir = opendir("/dev");
	if (!devdir)
		return NULL;
	while ((ent = readdir(devdir))) {
		if (strncmp(ent->d_name, "sg", 2) || !isdigit(ent->d_name[2]))
			continue;
		snprintf(devpath, PATH_MAX, "/dev/%s", ent->d_name);
		tmpfd = open(devpath, access_flags);
		if (tmpfd >= 0) {
			close(tmpfd);
			closedir(devdir);
			return devpath;
		}
	}
	closedir(devdir);
	return NULL;
}

void setup(void)
{
	const char *devpath = find_generic_scsi_device(O_RDONLY);
	if (!devpath)
		tst_brk(TCONF, "Could not find any usable SCSI device");
	tst_res(TINFO, "Found SCSI device %s", devpath);
	tst_pollute_memory(0, 0x42);
	devfd = open(devpath, O_RDONLY);
	query.interface_id = 'S';
	query.dxfer_direction = SG_DXFER_FROM_DEV;
	query.cmd_len = CMD_SIZE;
	query.dxfer_len = BUF_SIZE;
	query.dxferp = buffer;
	query.cmdp = command;
}

void cleanup(void)
{
	if (devfd >= 0)
		close(devfd);
}

void run(void)
{
	size_t i, j;
	memset(buffer, 0, BUF_SIZE);
	for (i = 0; i < 100; i++) {
ioctl(devfd, SG_IO, &query);
		if (TST_RET != 0 && TST_RET != -1)
			tst_brk(TBROK|TTERRNO, "Invalid ioctl() return value");
		for (j = 0; j < BUF_SIZE; j++) {
			if (buffer[j]) {
				tst_res(TFAIL, "Kernel memory leaked");
				return;
			}
		}
	}
	tst_res(TPASS, "Output buffer is empty, no data leaked");
}

void main(){
	setup();
	cleanup();
}
