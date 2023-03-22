#include "incl.h"
#define BUFSIZE 8192

const char MSG[] = "Ahoj!";

char *msg;

int map_fd;

uint32_t *key;

uint64_t *val;

char *log;

union bpf_attr *attr;

void ensure_ptr_arithmetic(void)
{
	const struct bpf_insn prog_insn[] = {
		 * r3 = -1
		 * r2 += r3
		 * *(char *)r2 = 0
		 */
		BPF_MOV64_REG(BPF_REG_2, BPF_REG_10),
		BPF_MOV64_IMM(BPF_REG_3, -1),
		BPF_ALU64_REG(BPF_ADD, BPF_REG_2, BPF_REG_3),
		BPF_ST_MEM(BPF_B, BPF_REG_2, 0, 0),
		BPF_MOV64_IMM(BPF_REG_0, 0),
		BPF_EXIT_INSN()
	};
	int ret;
	bpf_init_prog_attr(attr, prog_insn, sizeof(prog_insn), log, BUFSIZE);
	ret = TST_RETRY_FUNC(bpf(BPF_PROG_LOAD, attr, sizeof(*attr)),
				       TST_RETVAL_GE0);
	if (ret >= 0) {
		tst_res(TINFO, "Have pointer arithmetic");
		close(ret);
		return;
	}
	if (ret != -1)
		tst_brk(TBROK, "Invalid bpf() return value: %d", ret);
	if (log[0] != 0)
		tst_brk(TCONF | TERRNO, "No pointer arithmetic:\n %s", log);
	tst_brk(TBROK | TERRNO, "Failed to load program");
}

int load_prog(void)
{
	const struct bpf_insn prog_insn[] = {
		 * r7 = -1
		 */
		BPF_LD_IMM64(BPF_REG_6, 1ULL << 32),
		BPF_MOV64_IMM(BPF_REG_7, -1LL),
		BPF_ALU32_REG(BPF_DIV, BPF_REG_7, BPF_REG_6),
		 * map[2] = r7
		 */
		BPF_MAP_ARRAY_STX(map_fd, 0, BPF_REG_6),
		BPF_MAP_ARRAY_STX(map_fd, 1, BPF_REG_7),
		 * r7 = -1
		 */
		BPF_LD_IMM64(BPF_REG_6, 1ULL << 32),
		BPF_MOV64_IMM(BPF_REG_7, -1LL),
		BPF_ALU32_REG(BPF_MOD, BPF_REG_7, BPF_REG_6),
		 * map[4] = r7
		 */
		BPF_MAP_ARRAY_STX(map_fd, 2, BPF_REG_6),
		BPF_MAP_ARRAY_STX(map_fd, 3, BPF_REG_7),
		BPF_MOV64_IMM(BPF_REG_0, 0),
		BPF_EXIT_INSN()
	};
	bpf_init_prog_attr(attr, prog_insn, sizeof(prog_insn), log, BUFSIZE);
	return bpf_load_prog(attr, log);
}

void expect_reg_val(const char *const reg_name,
			   const uint64_t expected_val)
{
	bpf_map_array_get(map_fd, key, val);
	(*key)++;
	if (*val != expected_val) {
		tst_res(TFAIL,
			"%s = %"PRIu64", but should be %"PRIu64,
			reg_name, *val, expected_val);
	} else {
		tst_res(TPASS, "%s = %"PRIu64, reg_name, *val);
	}
}

void setup(void)
{
	rlimit_bump_memlock();
	memcpy(msg, MSG, sizeof(MSG));
}

void run(void)
{
	int prog_fd;
	map_fd = bpf_map_array_create(8);
	ensure_ptr_arithmetic();
	prog_fd = load_prog();
	bpf_run_prog(prog_fd, msg, sizeof(MSG));
	close(prog_fd);
	tst_res(TINFO, "Check w7(-1) /= w6(0) [r7 = -1, r6 = 1 << 32]");
	expect_reg_val("src(r6)", 1ULL << 32);
	expect_reg_val("dst(r7)", 0);
	tst_res(TINFO, "Check w7(-1) %%= w6(0) [r7 = -1, r6 = 1 << 32]");
	expect_reg_val("src(r6)", 1ULL << 32);
	expect_reg_val("dst(r7)", (uint32_t)-1);
	close(map_fd);
}

void main(){
	setup();
	cleanup();
}
