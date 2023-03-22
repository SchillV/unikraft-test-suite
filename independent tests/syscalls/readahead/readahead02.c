#include "incl.h"
#define _GNU_SOURCE

char testfile[PATH_MAX] = "testfile";
#define DROP_CACHES_FNAME "/proc/sys/vm/drop_caches"
#define MEMINFO_FNAME "/proc/meminfo"
#define PROC_IO_FNAME "/proc/self/io"
#define DEFAULT_FILESIZE (64 * 1024 * 1024)

size_t testfile_size = DEFAULT_FILESIZE;

char *opt_fsizestr;

int pagesize;

unsigned long cached_max;

int ovl_mounted;

int readahead_length  = 4096;

char sys_bdi_ra_path[PATH_MAX];

int orig_bdi_limit;

const char mntpoint[] = OVL_BASE_MNTPOINT;

int libc_readahead(int fd, off_t offset, size_t len)
{
	return readahead(fd, offset, len);
}

int fadvise_willneed(int fd, off_t offset, size_t len)
{
	errno = posix_fadvise(fd, offset, len, POSIX_FADV_WILLNEED);
	return errno ? -1 : 0;
}

struct tcase {
	const char *tname;
	int use_overlay;
	int use_fadvise;
	int (*readahead)(int fd, off_t offset, size_t len);
} tcases[] = {
	{ "readahead on file", 0, 0, libc_readahead },
	{ "readahead on overlayfs file", 1, 0, libc_readahead },
	{ "POSIX_FADV_WILLNEED on file", 0, 1, fadvise_willneed },
	{ "POSIX_FADV_WILLNEED on overlayfs file", 1, 1, fadvise_willneed },
};

int readahead_supported = 1;

int fadvise_supported = 1;

int has_file(const char *fname, int required)
{
	struct stat buf;
	if (stat(fname, &buf) == -1) {
		if (errno != ENOENT)
			tst_brk(TBROK | TERRNO, "stat %s", fname);
		if (required)
			tst_brk(TCONF, "%s not available", fname);
		return 0;
	}
	return 1;
}

void drop_caches(void)
{
	fprintf(DROP_CACHES_FNAME, "1");
}

unsigned long get_bytes_read(void)
{
	unsigned long ret;
	file_lines_scanf(PROC_IO_FNAME, "read_bytes: %lu", &ret);
	return ret;
}

unsigned long get_cached_size(void)
{
	unsigned long ret;
	file_lines_scanf(MEMINFO_FNAME, "Cached: %lu", &ret);
	return ret;
}

void create_testfile(int use_overlay)
{
	int fd;
	char *tmp;
	size_t i;
	sprintf(testfile, "%s/testfile",
		use_overlay ? OVL_MNT : OVL_BASE_MNTPOINT);
	tst_res(TINFO, "creating test file of size: %zu", testfile_size);
	tmp = malloc(pagesize);
	testfile_size = testfile_size & ~((long)pagesize - 1);
	fd = creat(testfile, 0644);
	for (i = 0; i < testfile_size; i += pagesize)
		write(1, fd, tmp, pagesize);
	fsync(fd);
	close(fd);
	free(tmp);
}

int read_testfile(struct tcase *tc, int do_readahead,
			 const char *fname, size_t fsize,
			 unsigned long *read_bytes, long long *usec,
			 unsigned long *cached)
{
	int fd;
	size_t i = 0;
	long read_bytes_start;
	unsigned char *p, tmp;
	off_t offset = 0;
	fd = open(fname, O_RDONLY);
	if (do_readahead) {
		do {
tc->readahead(fd, offset, fsize - offset);
			if (TST_RET != 0) {
				close(fd);
				return TST_ERR;
			}
			i++;
			offset += readahead_length;
		} while ((size_t)offset < fsize);
		tst_res(TINFO, "readahead calls made: %zu", i);
		*cached = get_cached_size();
		offset = lseek(fd, 0, SEEK_CUR);
		if (offset == 0)
			tst_res(TPASS, "offset is still at 0 as expected");
		else
			tst_res(TFAIL, "offset has changed to: %lu", offset);
	}
	tst_timer_start(CLOCK_MONOTONIC);
	read_bytes_start = get_bytes_read();
	p = mmap(NULL, fsize, PROT_READ, MAP_SHARED | MAP_POPULATE, fd, 0);
	tmp = 0;
	for (i = 0; i < fsize; i += pagesize)
		tmp = tmp ^ p[i];
	if (tmp != 0)
		tst_brk(TBROK, "This line should not be reached");
	if (!do_readahead)
		*cached = get_cached_size();
	munmap(p, fsize);
	tst_timer_stop();
	close(fd);
	return 0;
}

void test_readahead(unsigned int n)
{
	unsigned long read_bytes, read_bytes_ra;
	long long usec, usec_ra;
	unsigned long cached_high, cached_low, cached, cached_ra;
	int ret;
	struct tcase *tc = &tcases[n];
	tst_res(TINFO, "Test #%d: %s", n, tc->tname);
	if (tc->use_overlay && !ovl_mounted) {
		tst_res(TCONF, "overlayfs is not configured in this kernel");
		return;
	}
	create_testfile(tc->use_overlay);
	read_testfile(tc, 0, testfile, testfile_size, &read_bytes, &usec,
		      &cached);
	cached_high = get_cached_size();
	sync();
	drop_caches();
	cached_low = get_cached_size();
	cached_max = MAX(cached_max, cached_high - cached_low);
	tst_res(TINFO, "read_testfile(0)");
	read_testfile(tc, 0, testfile, testfile_size, &read_bytes, &usec,
		      &cached);
	if (cached > cached_low)
		cached = cached - cached_low;
	else
		cached = 0;
	sync();
	drop_caches();
	cached_low = get_cached_size();
	tst_res(TINFO, "read_testfile(1)");
	ret = read_testfile(tc, 1, testfile, testfile_size, &read_bytes_ra,
			    &usec_ra, &cached_ra);
	if (ret == EINVAL) {
		if (tc->use_fadvise &&
		    (!tc->use_overlay || !fadvise_supported)) {
			fadvise_supported = 0;
			tst_res(TCONF, "CONFIG_ADVISE_SYSCALLS not configured "
				"in kernel?");
			return;
		}
		if (!tc->use_overlay || !readahead_supported) {
			readahead_supported = 0;
			tst_res(TCONF, "readahead not supported on %s",
				tst_device->fs_type);
			return;
		}
	}
	if (ret) {
		tst_res(TFAIL | TTERRNO, "%s failed on %s",
			tc->use_fadvise ? "fadvise" : "readahead",
			tc->use_overlay ? "overlayfs" :
			tst_device->fs_type);
		return;
	}
	if (cached_ra > cached_low)
		cached_ra = cached_ra - cached_low;
	else
		cached_ra = 0;
	tst_res(TINFO, "read_testfile(0) took: %lli usec", usec);
	tst_res(TINFO, "read_testfile(1) took: %lli usec", usec_ra);
	if (has_file(PROC_IO_FNAME, 0)) {
		tst_res(TINFO, "read_testfile(0) read: %ld bytes", read_bytes);
		tst_res(TINFO, "read_testfile(1) read: %ld bytes",
			read_bytes_ra);
		if (read_bytes_ra < read_bytes)
			tst_res(TPASS, "readahead saved some I/O");
		else
			tst_res(TFAIL, "readahead failed to save any I/O");
	} else {
		tst_res(TCONF, "Your system doesn't have /proc/self/io,"
			" unable to determine read bytes during test");
	}
	tst_res(TINFO, "cache can hold at least: %ld kB", cached_max);
	tst_res(TINFO, "read_testfile(0) used cache: %ld kB", cached);
	tst_res(TINFO, "read_testfile(1) used cache: %ld kB", cached_ra);
	if (cached_max * 1024 >= testfile_size) {
		 * if cache can hold ~testfile_size then cache increase
		 * for readahead should be at least testfile_size/2
		 */
		if (cached_ra * 1024 > testfile_size / 2)
			tst_res(TPASS, "using cache as expected");
		else if (!cached_ra)
			tst_res(TFAIL, "readahead failed to use any cache");
		else
			tst_res(TWARN, "using less cache than expected");
	} else {
		tst_res(TCONF, "Page cache on your system is too small "
			"to hold whole testfile.");
	}
}

void setup_readahead_length(void)
{
	struct stat sbuf;
	char tmp[PATH_MAX], *backing_dev;
	int ra_new_limit, ra_limit;
	lstat(tst_device->dev, &sbuf);
	if (S_ISLNK(sbuf.st_mode))
		readlink(tst_device->dev, tmp, PATH_MAX);
	else
		strcpy(tmp, tst_device->dev);
	backing_dev = basename(tmp);
	sprintf(sys_bdi_ra_path, "/sys/class/block/%s/bdi/read_ahead_kb",
		backing_dev);
	if (access(sys_bdi_ra_path, F_OK))
		return;
	file_scanf(sys_bdi_ra_path, "%d", &orig_bdi_limit);
	ra_new_limit = testfile_size / 1024;
	while (ra_new_limit > pagesize / 1024) {
		FILE_PRINTF(sys_bdi_ra_path, "%d", ra_new_limit);
		file_scanf(sys_bdi_ra_path, "%d", &ra_limit);
		if (ra_limit == ra_new_limit) {
			readahead_length = MIN(ra_new_limit * 1024,
				2 * 1024 * 1024);
			break;
		}
		ra_new_limit = ra_new_limit / 2;
	}
}

void setup(void)
{
	if (opt_fsizestr) {
		testfile_size = strtol(opt_fsizestr, 1, INT_MAX);
		tst_set_max_runtime(1 + testfile_size / (DEFAULT_FILESIZE/32));
	}
	if (access(PROC_IO_FNAME, F_OK))
		tst_brk(TCONF, "Requires " PROC_IO_FNAME);
	has_file(DROP_CACHES_FNAME, 1);
	has_file(MEMINFO_FNAME, 1);
	tst_syscall(__NR_readahead, 0, 0, 0);
	pagesize = getpagesize();
	setup_readahead_length();
	tst_res(TINFO, "readahead length: %d", readahead_length);
	ovl_mounted = TST_MOUNT_OVERLAY();
}

void cleanup(void)
{
	if (ovl_mounted)
		umount(OVL_MNT);
	if (orig_bdi_limit)
		fprintf(sys_bdi_ra_path, "%d", orig_bdi_limit);
}

void main(){
	setup();
	cleanup();
}
