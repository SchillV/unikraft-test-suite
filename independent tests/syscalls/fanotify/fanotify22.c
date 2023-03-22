#include "incl.h"
#define _GNU_SOURCE
#ifdef HAVE_SYS_FANOTIFY_H
#ifndef EFSCORRUPTED
#endif
#define BUF_SIZE 256
#define MOUNT_PATH "test_mnt"
#define BASE_DIR "internal_dir"
#define BAD_DIR BASE_DIR"/bad_dir"
#ifdef HAVE_NAME_TO_HANDLE_AT

char event_buf[BUF_SIZE];

int fd_notify;

struct fanotify_fid_t null_fid;

struct fanotify_fid_t bad_file_fid;

void trigger_fs_abort(void)
{
	mount(tst_device->dev, MOUNT_PATH, tst_device->fs_type,
		   MS_REMOUNT|MS_RDONLY, "abort");
}

void do_debugfs_request(const char *dev, char *request)
{
	const char *const cmd[] = {"debugfs", "-w", dev, "-R", request, NULL};
	cmd(cmd, NULL, NULL);
}

void tcase2_trigger_lookup(void)
{
	int ret;
	ret = open(MOUNT_PATH"/"BAD_DIR, O_RDONLY, 0);
	if (ret != -1 && errno != EUCLEAN)
		tst_res(TFAIL, "Unexpected lookup result(%d) of %s (%d!=%d)",
			ret, BAD_DIR, errno, EUCLEAN);
}

void tcase3_trigger(void)
{
	trigger_fs_abort();
	tcase2_trigger_lookup();
}

void tcase4_trigger(void)
{
	tcase2_trigger_lookup();
	trigger_fs_abort();
}

struct test_case {
	char *name;
	int error;
	unsigned int error_count;
	struct fanotify_fid_t *fid;
	void (*trigger_error)(void);
} testcases[] = {
	{
		.name = "Trigger abort",
		.trigger_error = &trigger_fs_abort,
		.error_count = 1,
		.error = ESHUTDOWN,
		.fid = &null_fid,
	},
	{
		.name = "Lookup of inode with invalid mode",
		.trigger_error = &tcase2_trigger_lookup,
		.error_count = 1,
		.error = EFSCORRUPTED,
		.fid = &bad_file_fid,
	},
	{
		.name = "Multiple error submission",
		.trigger_error = &tcase3_trigger,
		.error_count = 2,
		.error = ESHUTDOWN,
		.fid = &null_fid,
	},
	{
		.name = "Multiple error submission 2",
		.trigger_error = &tcase4_trigger,
		.error_count = 2,
		.error = EFSCORRUPTED,
		.fid = &bad_file_fid,
	}
};

int check_error_event_info_fid(struct fanotify_event_info_fid *fid,
				 const struct test_case *ex)
{
	struct file_handle *fh = (struct file_handle *) &fid->handle;
	if (memcmp(&fid->fsid, &ex->fid->fsid, sizeof(fid->fsid))) {
		tst_res(TFAIL, "%s: Received bad FSID type (%x...!=%x...)",
			ex->name, FSID_VAL_MEMBER(fid->fsid, 0),
			ex->fid->fsid.val[0]);
		return 1;
	}
	if (fh->handle_type != ex->fid->handle.handle_type) {
		tst_res(TFAIL, "%s: Received bad file_handle type (%d!=%d)",
			ex->name, fh->handle_type, ex->fid->handle.handle_type);
		return 1;
	}
	if (fh->handle_bytes != ex->fid->handle.handle_bytes) {
		tst_res(TFAIL, "%s: Received bad file_handle len (%d!=%d)",
			ex->name, fh->handle_bytes, ex->fid->handle.handle_bytes);
		return 1;
	}
	if (memcmp(fh->f_handle, ex->fid->handle.f_handle, fh->handle_bytes)) {
		tst_res(TFAIL, "%s: Received wrong handle. "
			"Expected (%x...) got (%x...) ", ex->name,
			*(int *)ex->fid->handle.f_handle, *(int *)fh->f_handle);
		return 1;
	}
	return 0;
}

int check_error_event_info_error(struct fanotify_event_info_error *info_error,
				 const struct test_case *ex)
{
	int fail = 0;
	if (info_error->error_count != ex->error_count) {
		tst_res(TFAIL, "%s: Unexpected error_count (%d!=%d)",
			ex->name, info_error->error_count, ex->error_count);
		fail++;
	}
	if (info_error->error != ex->error) {
		tst_res(TFAIL, "%s: Unexpected error code value (%d!=%d)",
			ex->name, info_error->error, ex->error);
		fail++;
	}
	return fail;
}

int check_error_event_metadata(struct fanotify_event_metadata *event)
{
	int fail = 0;
	if (event->mask != FAN_FS_ERROR) {
		fail++;
		tst_res(TFAIL, "got unexpected event %llx",
			(unsigned long long)event->mask);
	}
	if (event->fd != FAN_NOFD) {
		fail++;
		tst_res(TFAIL, "Weird FAN_FD %llx",
			(unsigned long long)event->mask);
	}
	return fail;
}

void check_event(char *buf, size_t len, const struct test_case *ex)
{
	struct fanotify_event_metadata *event =
		(struct fanotify_event_metadata *) buf;
	struct fanotify_event_info_error *info_error;
	struct fanotify_event_info_fid *info_fid;
	int fail = 0;
	if (len < FAN_EVENT_METADATA_LEN) {
		tst_res(TFAIL, "No event metadata found");
		return;
	}
	if (check_error_event_metadata(event))
		return;
	info_error = get_event_info_error(event);
	if (info_error)
		fail += check_error_event_info_error(info_error, ex);
	else {
		tst_res(TFAIL, "Generic error record not found");
		fail++;
	}
	info_fid = get_event_info_fid(event);
	if (info_fid)
		fail += check_error_event_info_fid(info_fid, ex);
	else {
		tst_res(TFAIL, "FID record not found");
		fail++;
	}
	if (!fail)
		tst_res(TPASS, "Successfully received: %s", ex->name);
}

void do_test(unsigned int i)
{
	const struct test_case *tcase = &testcases[i];
	size_t read_len;
	fanotify_mark(fd_notify, FAN_MARK_ADD|FAN_MARK_FILESYSTEM,
			   FAN_FS_ERROR, AT_FDCWD, MOUNT_PATH);
	tcase->trigger_error();
	read_len = read(0, fd_notify, event_buf, BUF_SIZE);
	fanotify_mark(fd_notify, FAN_MARK_REMOVE|FAN_MARK_FILESYSTEM,
			   FAN_FS_ERROR, AT_FDCWD, MOUNT_PATH);
	check_event(event_buf, read_len, tcase);
}

void pre_corrupt_fs(void)
{
	mkdir(MOUNT_PATH"/"BASE_DIR, 0777);
	mkdir(MOUNT_PATH"/"BAD_DIR, 0777);
	fanotify_save_fid(MOUNT_PATH"/"BAD_DIR, &bad_file_fid);
	umount(MOUNT_PATH);
	do_debugfs_request(tst_device->dev, "sif " BAD_DIR " mode 0xff");
	mount(tst_device->dev, MOUNT_PATH, tst_device->fs_type, 0, NULL);
}

void init_null_fid(void)
{
	 * file_handler to create a null_fid
	 */
	fanotify_save_fid(MOUNT_PATH, &null_fid);
	null_fid.handle.handle_type = FILEID_INVALID;
	null_fid.handle.handle_bytes = 0;
}

void setup(void)
{
	REQUIRE_FANOTIFY_EVENTS_SUPPORTED_ON_FS(FAN_CLASS_NOTIF|FAN_REPORT_FID,
						FAN_MARK_FILESYSTEM,
						FAN_FS_ERROR, ".");
	pre_corrupt_fs();
	fd_notify = fanotify_init(FAN_CLASS_NOTIF|FAN_REPORT_FID,
				       O_RDONLY);
	init_null_fid();
}

void cleanup(void)
{
	if (fd_notify > 0)
		close(fd_notify);
}

void main(){
	setup();
	cleanup();
}
