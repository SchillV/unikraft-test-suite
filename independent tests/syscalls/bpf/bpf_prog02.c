#include "incl.h"
#define A64INT (((uint64_t)1) << 60)
const char MSG[] = "Ahoj!";

char *msg;

char *log;

uint32_t *key;

uint64_t *val;

union bpf_attr *attr;

int load_prog(int fd)
{
	struct bpf_insn insn[] = {
	};
	bpf_init_prog_attr(attr, insn, sizeof(insn), log, BUFSIZ);
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
	map_fd = bpf_map_array_create(2);
	prog_fd = load_prog(map_fd);
	bpf_run_prog(prog_fd, msg, sizeof(MSG));
	close(prog_fd);
        *key = 0;
	bpf_map_array_get(map_fd, key, val);
	if (*val != A64INT + 1) {
		tst_res(TFAIL,
			"val = %"PRIu64", but should be val = %"PRIu64" + 1",
			*val, A64INT);
	} else {
		tst_res(TPASS, "val = %"PRIu64" + 1", A64INT);
	}
	bpf_map_array_get(map_fd, key, val);
	if (*val != A64INT - 1) {
		tst_res(TFAIL,
			"val = %"PRIu64", but should be val = %"PRIu64" - 1",
			*val, A64INT);
	} else {
		tst_res(TPASS, "val = %"PRIu64" - 1", A64INT);
	}
	close(map_fd);
}

void main(){
	setup();
	cleanup();
}
