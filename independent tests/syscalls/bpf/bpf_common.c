#include "incl.h"
#define TST_NO_DEFAULT_MAIN
void rlimit_bump_memlock(void)
{
	struct rlimit memlock_r;
	getrlimit(RLIMIT_MEMLOCK, &memlock_r);
	memlock_r.rlim_cur += BPF_MEMLOCK_ADD;
	tst_res(TINFO, "Raising RLIMIT_MEMLOCK to %ld",
		(long)memlock_r.rlim_cur);
	if (memlock_r.rlim_cur <= memlock_r.rlim_max) {
		setrlimit(RLIMIT_MEMLOCK, &memlock_r);
	} else if ((geteuid() == 0)) {
		memlock_r.rlim_max += BPF_MEMLOCK_ADD;
		setrlimit(RLIMIT_MEMLOCK, &memlock_r);
	} else {
		tst_res(TINFO, "Can't raise RLIMIT_MEMLOCK, test may fail "
			"due to lack of max locked memory");
	}
}
int bpf_map_create(union bpf_attr *const attr)
{
	int ret;
	ret = TST_RETRY_FUNC(bpf(BPF_MAP_CREATE, attr, sizeof(*attr)),
		TST_RETVAL_GE0);
	if (ret == -1) {
		if (errno == EPERM) {
			tst_res(TCONF, "Hint: check also /proc/sys/kernel/unprivileged_bpf_disabled");
			tst_brk(TCONF | TERRNO,
				"bpf() requires CAP_SYS_ADMIN or CAP_BPF on this system");
		} else {
			tst_brk(TBROK | TERRNO, "Failed to create array map");
		}
	}
	return ret;
}
int bpf_map_array_create(const uint32_t max_entries)
{
	union bpf_attr map_attr = {
		.map_type = BPF_MAP_TYPE_ARRAY,
		.key_size = 4,
		.value_size = 8,
		.max_entries = max_entries,
		.map_flags = 0
	};
	return bpf_map_create(&map_attr);
}
void bpf_map_array_get(const int map_fd,
		       const uint32_t *const array_indx,
		       uint64_t *const array_val)
{
	union bpf_attr elem_attr = {
		.map_fd = map_fd,
		.key = ptr_to_u64(array_indx),
		.value = ptr_to_u64(array_val),
		.flags = 0
	};
	const int ret = bpf(BPF_MAP_LOOKUP_ELEM, &elem_attr, sizeof(elem_attr));
	if (ret) {
		tst_brk(TBROK | TTERRNO,
			"Failed array map lookup: fd=%i [%"PRIu32"]",
			map_fd, *array_indx);
	}
}
void bpf_init_prog_attr(union bpf_attr *const attr,
			const struct bpf_insn *const prog,
			const size_t prog_size, char *const log_buf,
			const size_t log_size)
{
	

struct bpf_insn *buf;
	

size_t buf_size;
	const size_t prog_len = prog_size / sizeof(*prog);
	if (!buf || prog_size > buf_size) {
		buf = tst_alloc(prog_size);
		buf_size = prog_size;
	}
	memcpy(buf, prog, prog_size);
	memset(attr, 0, sizeof(*attr));
	attr->prog_type = BPF_PROG_TYPE_SOCKET_FILTER;
	attr->insns = ptr_to_u64(buf);
	attr->insn_cnt = prog_len;
	attr->license = ptr_to_u64("GPL");
	attr->log_buf = ptr_to_u64(log_buf);
	attr->log_size = log_size;
	attr->log_level = 1;
}
int bpf_load_prog(union bpf_attr *const attr, const char *const log)
{
	const int ret = TST_RETRY_FUNC(bpf(BPF_PROG_LOAD, attr, sizeof(*attr)),
				       TST_RETVAL_GE0);
	if (ret >= 0) {
		tst_res(TPASS, "Loaded program");
		return ret;
	}
	if (ret != -1)
		tst_brk(TBROK, "Invalid bpf() return value: %d", ret);
	if (log[0] != 0) {
		tst_printf("%s\n", log);
		tst_brk(TBROK | TERRNO, "Failed verification");
	}
	tst_brk(TBROK | TERRNO, "Failed to load program");
	return ret;
}
void bpf_run_prog(const int prog_fd,
		  const char *const msg, const size_t msg_len)
{
	int sk[2];
	socketpair(AF_UNIX, SOCK_DGRAM, 0, sk);
	setsockopt(sk[1], SOL_SOCKET, SO_ATTACH_BPF,
			&prog_fd, sizeof(prog_fd));
	write(1, sk[0], msg, msg_len);
	close(sk[0]);
	close(sk[1]);
}

void main(){
	setup();
	cleanup();
}
