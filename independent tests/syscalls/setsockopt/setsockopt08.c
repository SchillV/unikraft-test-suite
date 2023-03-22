#include "incl.h"

void *buffer;
void setup(void)
{
	if (tst_kernel_bits() == 32 || sizeof(long) > 4) {
		tst_res(TINFO,
			"The vulnerability was only present in 32-bit compat mode");
	}
	tst_setup_netns();
}
void run(void)
{
	const char *const res_fmt_str =
		"setsockopt(%d, IPPROTO_IP, IPT_SO_SET_REPLACE, %p, 1)";
	struct ipt_replace *ipt_replace = buffer;
	struct ipt_entry *ipt_entry = &ipt_replace->entries[0];
	struct xt_entry_match *xt_entry_match =
		(struct xt_entry_match *)&ipt_entry->elems[0];
	const size_t tgt_size = 32;
	const size_t match_size = 1024 - 64 - 112 - 4 - tgt_size - 4;
	struct xt_entry_target *xt_entry_tgt =
		((struct xt_entry_target *) (&ipt_entry->elems[0] + match_size));
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	int result;
	xt_entry_match->u.user.match_size = (u_int16_t)match_size;
	strcpy(xt_entry_match->u.user.name, "state");
	xt_entry_tgt->u.user.target_size = (u_int16_t)tgt_size;
	strcpy(xt_entry_tgt->u.user.name, "REJECT");
	ipt_entry->target_offset =
		(__builtin_offsetof(struct ipt_entry, elems) + match_size);
	ipt_entry->next_offset = ipt_entry->target_offset + tgt_size;
	strcpy(ipt_replace->name, "filter");
	ipt_replace->num_entries = 1;
	ipt_replace->num_counters = 1;
	ipt_replace->size = ipt_entry->next_offset;
setsockopt(fd, IPPROTO_IP, IPT_SO_SET_REPLACE, buffer, 1);
	if (TST_RET == -1 && TST_ERR == ENOPROTOOPT)
		tst_brk(TCONF | TTERRNO, res_fmt_str, fd, buffer);
	result = (TST_RET == -1 && TST_ERR == EINVAL) ? TPASS : TFAIL;
	tst_res(result | TTERRNO, res_fmt_str, fd, buffer);
	close(fd);
}

void main(){
	setup();
	cleanup();
}
