#include "incl.h"
#define BUFSIZE 8192
#define CHECK_BPF_RET(x) ((x) >= 0 || ((x) == -1 && errno != EACCES))

const char MSG[] = "Ahoj!";

char *msg;

char *log;

union bpf_attr *attr;

int load_prog(int fd)
{
	int ret;
	struct bpf_insn insn[] = {
		BPF_MOV64_IMM(BPF_REG_8, 2),
		BPF_ALU64_IMM(BPF_LSH, BPF_REG_8, 31),
		BPF_ALU32_IMM(BPF_RSH, BPF_REG_8, 31),
		BPF_ALU32_IMM(BPF_SUB, BPF_REG_8, 2),
		BPF_LD_MAP_FD(BPF_REG_1, fd),
		BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
		BPF_ALU64_IMM(BPF_ADD, BPF_REG_2, -4),
		BPF_ST_MEM(BPF_W, BPF_REG_2, 0, 0),
		BPF_EMIT_CALL(BPF_FUNC_map_lookup_elem),
		BPF_JMP_IMM(BPF_JNE, BPF_REG_0, 0, 1),
		BPF_EXIT_INSN(),
		BPF_ALU64_REG(BPF_ADD, BPF_REG_0, BPF_REG_8),
		BPF_STX_MEM(BPF_DW, BPF_REG_0, BPF_REG_8, 0),
		BPF_MOV64_IMM(BPF_REG_0, 0),
		BPF_EXIT_INSN()
	};
	bpf_init_prog_attr(attr, insn, sizeof(insn), log, BUFSIZE);
	ret = TST_RETRY_FUNC(bpf(BPF_PROG_LOAD, attr, sizeof(*attr)),
		CHECK_BPF_RET);
	if (ret >= 0) {
		tst_res(TINFO, "Verification log:");
		fputs(log, stderr);
		return ret;
	}
	if (ret < -1)
		tst_brk(TBROK, "Invalid bpf() return value %d", ret);
	if (!*log)
		tst_brk(TBROK | TERRNO, "Failed to load BPF program");
	tst_res(TPASS | TERRNO, "BPF program failed verification");
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
	map_fd = bpf_map_array_create(1);
	prog_fd = load_prog(map_fd);
	if (prog_fd >= 0) {
		tst_res(TFAIL, "Malicious eBPF code passed verification. "
			"Now let's try crashing the kernel.");
		bpf_run_prog(prog_fd, msg, sizeof(MSG));
	}
	if (prog_fd >= 0)
		close(prog_fd);
	close(map_fd);
}

void main(){
	setup();
	cleanup();
}
