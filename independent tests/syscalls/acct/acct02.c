#include "incl.h"
#define COMMAND		"acct02_helper"
#define OUTPUT_FILE	"acct_file"
#define UNPACK(x) ((x & 0x1fff) << (((x >> 13) & 0x7) * 3))
#define ACCT_MEMBER(x) (v3 ? ((struct acct_v3 *)acc)->x : ((struct acct *)acc)->x)
#define ACCT_MEMBER_V3(x) (((struct acct_v3 *)acc)->x)

int fd;

int v3;

int acct_size;

int clock_ticks;

unsigned int rc;

unsigned int start_time;

union acct_union {
	struct acct	v0;
	struct acct_v3	v3;
} acct_struct;
#define ACCT_V3 "CONFIG_BSD_PROCESS_ACCT_V3"

int acct_version_is_3(void)
{
	struct tst_kconfig_var kconfig = {
		.id = ACCT_V3,
		.id_len = sizeof(ACCT_V3)-1,
	};
	tst_kconfig_read(&kconfig, 1);
	tst_res(TINFO, ACCT_V3 "=%c", kconfig.choice);
	return kconfig.choice == 'y';
}

void run_command(void)
{
	const char *const cmd[] = {COMMAND, NULL};
	rc = tst_cmd(cmd, NULL, NULL, TST_CMD_PASS_RETVAL) << 8;
}

int  *acc, int elap_time)
{
	int sys_time  = UNPACK(ACCT_MEMBER(ac_stime));
	int user_time = UNPACK(ACCT_MEMBER(ac_stime));
	unsigned int btime_diff;
	int ret = 0;
	float tmp2;
	if (strcmp(ACCT_MEMBER(ac_comm), COMMAND)) {
		tst_res(TINFO, "ac_comm != '%s' ('%s')", COMMAND,
			ACCT_MEMBER(ac_comm));
		ret = 1;
	}
	if (start_time > ACCT_MEMBER(ac_btime))
		btime_diff = start_time - ACCT_MEMBER(ac_btime);
	else
		btime_diff = ACCT_MEMBER(ac_btime) - start_time;
	if (btime_diff > 7200) {
		tst_res(TINFO, "ac_btime_diff %u", btime_diff);
		ret = 1;
	}
	if (ACCT_MEMBER(ac_uid) != getuid()) {
		tst_res(TINFO, "ac_uid != %d (%d)", getuid(),
			ACCT_MEMBER(ac_uid));
		ret = 1;
	}
	if (ACCT_MEMBER(ac_gid) != getgid()) {
		tst_res(TINFO, "ac_gid != %d (%d)", getgid(),
			ACCT_MEMBER(ac_gid));
		ret = 1;
	}
	tmp2 = user_time/clock_ticks;
	if (tmp2 > 1) {
		tst_res(TINFO, "user_time/clock_ticks > 1 (%d/%d: %.2f)",
			user_time, clock_ticks, tmp2);
		ret = 1;
	}
	tmp2 = sys_time/clock_ticks;
	if (tmp2 > 1) {
		tst_res(TINFO, "sys_time/clock_ticks > 1 (%d/%d: %.2f)",
			sys_time, clock_ticks, tmp2);
		ret = 1;
	}
	tmp2 = elap_time/clock_ticks;
	if (tmp2 >= 2) {
		tst_res(TINFO, "elap_time/clock_ticks >= 2 (%d/%d: %.2f)",
			elap_time, clock_ticks, tmp2);
		ret = 1;
	}
	if (ACCT_MEMBER(ac_exitcode) != rc) {
		tst_res(TINFO, "ac_exitcode != %d (%d)", rc,
			ACCT_MEMBER(ac_exitcode));
		ret = 1;
	}
	if (!v3)
		return ret;
	if (ACCT_MEMBER_V3(ac_ppid) != (uint32_t)getpid()) {
		tst_res(TINFO, "ac_ppid != %d (%d)", (uint32_t)getpid(),
			ACCT_MEMBER_V3(ac_ppid));
		ret = 1;
	}
	if (ACCT_MEMBER_V3(ac_version) != (char)(3 | ACCT_BYTEORDER)) {
		tst_res(TINFO, "ac_version != 3 (%d)",
			ACCT_MEMBER_V3(ac_version));
		ret = 1;
	}
	if (ACCT_MEMBER_V3(ac_pid) < 1) {
		tst_res(TINFO, "ac_pid < 1 (%d)", ACCT_MEMBER_V3(ac_pid));
		ret = 1;
	}
	return ret;
}

void run(void)
{
	int read_bytes, ret;
	int entry_count = 0, i = 0;
	fd = open(OUTPUT_FILE, O_RDWR | O_CREAT, 0644);
acct(OUTPUT_FILE);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "Could not set acct output file");
	start_time = time(NULL);
	run_command();
	acct(NULL);
	do {
		read_bytes = read(0, fd, &acct_struct, acct_size);
		if (i == 0 && read_bytes == 0) {
			tst_res(TFAIL, "acct file is empty");
			goto exit;
		}
		if (read_bytes == 0) {
			tst_res(TFAIL, "end of file reached");
			goto exit;
		}
		if (read_bytes != acct_size) {
			tst_res(TFAIL, "incomplete read %i bytes, expected %i",
			        read_bytes, acct_size);
			goto exit;
		}
		tst_res(TINFO, "== entry %d ==", ++i);
		if (v3)
int 			ret = verify_acct(&acct_struct.v3, acct_struct.v3.ac_etime);
		else
int 			ret = verify_acct(&acct_struct.v0, UNPACK(acct_struct.v0.ac_etime));
		if (read_bytes)
			entry_count++;
	} while (read_bytes == acct_size && ret);
	tst_res(TINFO, "Number of accounting file entries tested: %d",
			entry_count);
	if (ret)
		tst_res(TFAIL, "acct() wrote incorrect file contents!");
	else
		tst_res(TPASS, "acct() wrote correct file contents!");
exit:
	close(fd);
}

void setup(void)
{
	struct statfs buf;
	clock_ticks = sysconf(_SC_CLK_TCK);
	statfs(".", &buf);
	float avail = (100.00 * buf.f_bavail) / buf.f_blocks;
	if (avail < 4.1) {
		tst_brk(TCONF,
			"Less than 4.1%% (%.2f) of free space on filesystem",
			avail);
	}
acct(NULL);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO,
			"acct() system call returned with error");
	v3 = acct_version_is_3();
	if (v3) {
		tst_res(TINFO, "Verifying using 'struct acct_v3'");
		acct_size = sizeof(struct acct_v3);
	} else {
		tst_res(TINFO, "Verifying using 'struct acct'");
		acct_size = sizeof(struct acct);
	}
}

void cleanup(void)
{
	if (fd > 0)
		close(fd);
	acct(NULL);
}

void main(){
	setup();
	cleanup();
}
