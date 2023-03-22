#include "incl.h"

int  verify_brk()
{
	void *cur_brk, *new_brk;
	size_t inc = getpagesize() * 2 - 1;
	unsigned int i;
	printf("[I] Testing libc variant\n");
	cur_brk = (void *)sbrk(0);
	if (cur_brk == (void *)-1){
		printf("sbrk() not implemented\n");
		return 0;
	}
	if (brk(cur_brk) != 0){
		printf("brk() not implemented\n");
		return 0;
	}
	for (i = 0; i < 33; i++) {
		switch (i % 3) {
		case 0:
			new_brk = cur_brk + inc;
		break;
		case 1:
			new_brk = cur_brk;
		break;
		case 2:
			new_brk = cur_brk - inc;
		break;
		}
		brk(new_brk);
		cur_brk = sbrk(0);
		if (cur_brk != new_brk) {
			printf("brk() failed to set address have %p expected %p\n", cur_brk, new_brk);
			return 0;
		}
		if (i % 3 == 0)
			*((char *)cur_brk) = 0;
	}
	printf("brk() works fine\n");
	return 1;
}

void main(){
	if(verify_brk())
		printf("test succeeded\n");
}
