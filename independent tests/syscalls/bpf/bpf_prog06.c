#include "incl.h"
#define BUFSIZE 8192

const char MSG[] = "Ahoj!";

char *msg;

int map_fd;

uint32_t *key;

uint64_t *val;

char *log;

union bpf_attr *attr;

int load_prog(void)
{
	int ret;
	const struct bpf_insn prog_insn[] = {
		BPF_LD_MAP_FD(BPF_REG_1, map_fd),
		BPF_MOV64_IMM(BPF_REG_2, 0xff0),
		BPF_MOV64_IMM(BPF_REG_3, 0x00),
		BPF_EMIT_CALL(BPF_FUNC_ringbuf_reserve),
		BPF_JMP_IMM(BPF_JNE, BPF_REG_0, 0, 2),
		BPF_MOV64_IMM(BPF_REG_0, 2),
		BPF_EXIT_INSN(),
		BPF_ALU64_IMM(BPF_SUB, BPF_REG_0, (0x3008-0x38)),
		BPF_MOV64_REG(BPF_REG_1, BPF_REG_0),
		BPF_MOV64_IMM(BPF_REG_2, 1),
		BPF_EMIT_CALL(BPF_FUNC_ringbuf_submit),
		BPF_MOV64_IMM(BPF_REG_0, 0),
		BPF_EXIT_INSN()
	};
	bpf_init_prog_attr(attr, prog_insn, sizeof(prog_insn), log, BUFSIZE);
	ret = TST_RETRY_FUNC(bpf(BPF_PROG_LOAD, attr, sizeof(*attr)),
			     TST_RETVAL_GE0);
	if (ret >= 0)
		return ret;
	if (ret != -1)
		tst_brk(TBROK, "Invalid bpf() return value: %d", ret);
	if (log[0] != 0)
		tst_printf("%s\n", log);
	return ret;
}

void setup(void)
{
	rlimit_bump_memlock();
	memcpy(msg, MSG, sizeof(MSG));
}

void run(void)
{
	int prog_fd;
	map_fd = bpf_map_create(&(union bpf_attr){
			.map_type = BPF_MAP_TYPE_RINGBUF,
			.key_size = 0,
			.value_size = 0,
			.max_entries = getpagesize()
		});
	tst_res(TINFO, "Trying to load eBPF with OOB write");
	prog_fd = load_prog();
	if (prog_fd == -1) {
		tst_res(TPASS, "Failed verification");
		return;
	}
	tst_res(TFAIL, "Loaded program with OOB write");
	tst_res(TINFO, "Running eBPF with OOB");
	bpf_run_prog(prog_fd, msg, sizeof(MSG));
	tst_res(TINFO, "Ran eBPF");
	close(prog_fd);
}

void main(){
	setup();
	cleanup();
}
