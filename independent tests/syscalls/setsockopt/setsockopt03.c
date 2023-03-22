#include "incl.h"
#define TOO_SMALL_OFFSET 74
#define OFFSET_OVERWRITE 0xFFFF
#define NEXT_OFFSET (sizeof(struct ipt_entry)		\
		     + sizeof(struct xt_entry_match)	\
		     + sizeof(struct xt_entry_target))
#define PADDING (OFFSET_OVERWRITE - NEXT_OFFSET)
struct payload {
	struct ipt_replace repl;
	struct ipt_entry ent;
	struct xt_entry_match match;
	struct xt_entry_target targ;
	char padding[PADDING];
	struct xt_entry_target targ2;
};

void setup(void)
{
	if (tst_kernel_bits() == 32 || sizeof(long) > 4)
		tst_res(TCONF,
			"The vulnerability was only present in 32-bit compat mode");
}

void run(void)
{
	int ret, sock_fd;
	struct payload p = { 0 };
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(p.match.u.user.name, "icmp", sizeof(p.match.u.user.name));
	p.match.u.match_size = OFFSET_OVERWRITE;
	p.ent.next_offset = NEXT_OFFSET;
	p.ent.target_offset = TOO_SMALL_OFFSET;
	p.repl.num_entries = 2;
	p.repl.num_counters = 1;
	p.repl.size = sizeof(struct payload);
	p.repl.valid_hooks = 0;
	ret = setsockopt(sock_fd, SOL_IP, IPT_SO_SET_REPLACE,
			 &p, sizeof(struct payload));
	tst_res(TPASS | TERRNO, "We didn't cause a crash, setsockopt returned %d", ret);
}

void main(){
	setup();
	cleanup();
}
