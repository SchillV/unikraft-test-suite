#include "incl.h"
const char MSG[] = "Ahoj!";

char *msg;

char *log;

union bpf_attr *attr;
int load_prog(int fd)
{
	 * The following is a byte code template. We copy it to a guarded buffer and
	 * substitute the runtime value of our map file descriptor.
	 *
	 * r0 - r10 = registers 0 to 10
	 * r0 = return code
	 * r1 - r5 = scratch registers, used for function arguments
	 * r6 - r9 = registers preserved across function calls
	 * fp/r10 = stack frame pointer
	 */
	struct bpf_insn PROG[] = {
		BPF_LD_MAP_FD(BPF_REG_1, fd),
		BPF_EMIT_CALL(BPF_FUNC_map_lookup_elem),
		BPF_JMP_IMM(BPF_JEQ, BPF_REG_0, 0, 3),
	};
	bpf_init_prog_attr(attr, PROG, sizeof(PROG), log, BUFSIZ);
	return bpf_load_prog(attr, log);
}
void setup(void)
{
	rlimit_bump_memlock();
	memcpy(msg, MSG, sizeof(MSG));
}
void run(void)
{
	int map_fd, prog_fd;
	uint32_t key = 0;
	uint64_t val;
	map_fd = bpf_map_array_create(1);
	prog_fd = load_prog(map_fd);
	bpf_run_prog(prog_fd, msg, sizeof(MSG));
	close(prog_fd);
	bpf_map_array_get(map_fd, &key, &val);
	if (val != 1) {
		tst_res(TFAIL,
			"val = %lu, but should be val = 1",
			val);
        } else {
	        tst_res(TPASS, "val = 1");
	}
	close(map_fd);
}

void main(){
	setup();
	cleanup();
}
