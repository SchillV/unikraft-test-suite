#include "incl.h"
#define PROC_ENTROPY_AVAIL "/proc/sys/kernel/random/entropy_avail"

int modes[] = { 0, GRND_RANDOM, GRND_NONBLOCK,
		       GRND_RANDOM | GRND_NONBLOCK };

int check_content(unsigned char *buf, int nb)
{
	int table[256];
	int i, index, max;
	memset(table, 0, sizeof(table));
	max = 6 + nb * 0.2;
	for (i = 0; i < nb; i++) {
		index = buf[i];
		table[index]++;
	}
	for (i = 0; i < nb; i++) {
		if (max > 0 && table[i] > max)
			return 0;
	}
	return 1;
}

int  verify_getrandom(unsigned int n)
{
	unsigned char buf[256];
	int bufsize = 64, entropy_avail;
	if (access(PROC_ENTROPY_AVAIL, F_OK) == 0) {
		file_scanf(PROC_ENTROPY_AVAIL, "%d", &entropy_avail);
		if (entropy_avail > 256)
			bufsize = sizeof(buf);
	}
	memset(buf, 0, sizeof(buf));
	do {
tst_syscall(__NR_getrandom, buf, bufsize, modes[n]);
	} while ((modes[n] & GRND_NONBLOCK) && TST_RET == -1
		  && TST_ERR == EAGAIN);
	if (!check_content(buf, TST_RET))
		tst_res(TFAIL | TTERRNO, "getrandom failed");
	else
		tst_res(TPASS, "getrandom returned %ld", TST_RET);
}

void main(){
	setup();
	cleanup();
}
