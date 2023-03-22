#include "incl.h"
#define LOG_SIZE (1024 * 1024)
#define CHECK_BPF_RET(x) ((x) >= 0 || ((x) == -1 && errno != EPERM))

const char MSG[] = "Ahoj!";

char *msg;

char *log;

uint32_t *key;

uint64_t *val;

union bpf_attr *attr;

int load_prog(int fd)
{
	int ret;
	struct bpf_insn insn[] = {
		BPF_LD_MAP_FD(BPF_REG_1, fd),
		BPF_MOV64_REG(BPF_REG_8, BPF_REG_10),
		BPF_MOV32_IMM(BPF_REG_2, 0),
		BPF_STX_MEM(BPF_W, BPF_REG_8, BPF_REG_2, 0),
		BPF_MOV64_REG(BPF_REG_2, BPF_REG_8),
		BPF_EMIT_CALL(BPF_FUNC_map_lookup_elem),
		BPF_JMP_IMM(BPF_JNE, BPF_REG_0, 0, 2),
		BPF_MOV32_IMM(BPF_REG_1, -1),
		BPF_ALU64_IMM(BPF_ADD, BPF_REG_1, 1),
		BPF_ALU64_IMM(BPF_RSH, BPF_REG_1, 26),
		BPF_STX_MEM(BPF_DW, BPF_REG_0, BPF_REG_1, 0),
		BPF_MOV64_REG(BPF_REG_2, BPF_REG_0),
		BPF_ALU64_REG(BPF_SUB, BPF_REG_2, BPF_REG_1),
		BPF_MOV64_IMM(BPF_REG_3, 0xdeadbeef),
		BPF_STX_MEM(BPF_DW, BPF_REG_2, BPF_REG_3, 0),
		BPF_ALU64_REG(BPF_SUB, BPF_REG_2, BPF_REG_1),
		BPF_STX_MEM(BPF_DW, BPF_REG_2, BPF_REG_3, 0),
		BPF_ALU64_REG(BPF_SUB, BPF_REG_2, BPF_REG_1),
		BPF_STX_MEM(BPF_DW, BPF_REG_2, BPF_REG_3, 0),
		BPF_ALU64_REG(BPF_SUB, BPF_REG_2, BPF_REG_1),
		BPF_STX_MEM(BPF_DW, BPF_REG_2, BPF_REG_3, 0),
		BPF_MOV32_IMM(BPF_REG_0, 0),
		BPF_EXIT_INSN()
	};
	bpf_init_prog_attr(attr, insn, sizeof(insn), log, LOG_SIZE);
	ret = TST_RETRY_FUNC(bpf(BPF_PROG_LOAD, attr, sizeof(*attr)),
		CHECK_BPF_RET);
	if (ret < -1)
		tst_brk(TBROK, "Invalid bpf() return value %d", ret);
	if (ret >= 0) {
		tst_res(TINFO, "Verification log:");
		fputs(log, stderr);
		return ret;
	}
	if (log[0] == 0)
		tst_brk(TBROK | TERRNO, "Failed to load program");
	tst_res(TPASS | TERRNO, "Failed verification");
	return ret;
}

void setup(void)
{
	rlimit_bump_memlock();
	memcpy(msg, MSG, sizeof(MSG));
}

void run(void)
{
	int map_fd, prog_fd;
	map_fd = bpf_map_array_create(32);
	memset(attr, 0, sizeof(*attr));
	attr->map_fd = map_fd;
	attr->key = ptr_to_u64(key);
	attr->value = ptr_to_u64(val);
	attr->flags = BPF_ANY;
bpf(BPF_MAP_UPDATE_ELEM, attr, sizeof(*attr));
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "Failed to lookup map element");
	prog_fd = load_prog(map_fd);
	if (prog_fd == -1)
		goto exit;
	tst_res(TFAIL, "Loaded bad eBPF, now we will run it and maybe crash");
	bpf_run_prog(prog_fd, msg, sizeof(MSG));
	close(prog_fd);
	bpf_map_array_get(map_fd, key, val);
	tst_res(TINFO, "Pointer offset was 0x%"PRIx64, *val);
exit:
	close(map_fd);
}

void main(){
	setup();
	cleanup();
}
